#include "DevConsole.h"
#include <algorithm>
#include <Imgui/imgui.h>

SE::Core::DevConsole::DevConsole()
{
	showConsole = false;
	inputBuffer.resize(256);
	commandHistoryIndex = -1;
	AddCommand(
		[this](int argc, char** argv)
	{
		std::string toPrint = "";
		for (auto& c : nameToCommand)
		{
			Print(c.second.name + "\n" + c.second.description);
		}
	},
		"commands",
		"Lists all availible commands."

		);
}

SE::Core::DevConsole::~DevConsole()
{
}

int SE::Core::DevConsole::AddCommand(const ConsoleCommand & command, const std::string & name, const std::string & description)
{
	auto find = nameToCommand.find(name);
	if (find != nameToCommand.end())
		return -1;
	nameToCommand[name] = { name, description, command };
	return 0;
}

void SE::Core::DevConsole::Print(const std::string & message, const std::string & channel)
{
	messages.push_back({ std::chrono::system_clock::now(), message, channel });
}

void SE::Core::DevConsole::Show()
{
	showConsole = true;
}

void SE::Core::DevConsole::Hide()
{
	showConsole = false;
}

void SE::Core::DevConsole::Toggle()
{
	showConsole = !showConsole;
}

void SE::Core::DevConsole::Frame()
{
	if (!showConsole)
		return;
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Dev console", &showConsole))
	{
		ImGui::End();
		return;
	}
	static ImGuiTextFilter filter;
	filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
	ImGui::Separator();
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
	for (auto& m : messages)
	{
		ImVec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		if (m.channel == "Error")
			color = { 1.0f, 0.2f, 0.2f, 1.0f };
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::TextUnformatted((m.channel + ": " +m.message).c_str());
		ImGui::PopStyleColor();
	}
	ImGui::EndChild();
	ImGui::Separator();
	if (ImGui::InputText("Input", inputBuffer.data(), inputBuffer.size(), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub, (void*)this))
	{
		char* input_end = inputBuffer.data() + strlen(inputBuffer.data());
		while (input_end > inputBuffer.data() && input_end[-1] == ' ') { input_end--; } *input_end = 0;
		if (inputBuffer[0])
		{
			std::string commandstring = std::string(inputBuffer.data(), input_end - inputBuffer.data());
			ExecuteCommand(commandstring);
			commandHistory.push_back(commandstring);
		}
		inputBuffer[0] = '\0';
	}

	// Demonstrate keeping auto focus on the input box
	if (ImGui::IsItemHovered() || (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
		ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
	ImGui::End();
}

int SE::Core::DevConsole::TextEditCallbackStub(ImGuiTextEditCallbackData * data)
{
	DevConsole* devcon = (DevConsole*)data->UserData;
	return devcon->TextEditCallback(data);
}

int SE::Core::DevConsole::TextEditCallback(ImGuiTextEditCallbackData * data)
{
	//AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
	switch (data->EventFlag)
	{
	case ImGuiInputTextFlags_CallbackCompletion:
	{
		// Example of TEXT COMPLETION

		// Locate beginning of current word
		const char* word_end = data->Buf + data->CursorPos;
		const char* word_start = word_end;
		while (word_start > data->Buf)
		{
			const char c = word_start[-1];
			if (c == ' ' || c == '\t' || c == ',' || c == ';')
				break;
			word_start--;
		}
		int word_len = (int)(word_end - word_start);
		std::string wordStr = std::string(word_start, word_len);
		// Build a list of candidates
		std::vector<std::string> candidates;
		for (auto& c : nameToCommand)
		{
			std::string part = c.second.name.substr(0, word_len);
			if (part == wordStr)
				candidates.push_back(c.second.name);
		}
		

		if (candidates.size() == 1)
		{
			// Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
			data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
			data->InsertChars(data->CursorPos, candidates[0].c_str());
			data->InsertChars(data->CursorPos, " ");
		}
		else
		{
			// Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
			int match_len = (int)(word_end - word_start);
			for (;;)
			{
				int c = 0;
				bool all_candidates_matches = true;
				for (int i = 0; i < candidates.size() && all_candidates_matches; i++)
					if (i == 0)
						c = toupper(candidates[i][match_len]);
					else if (c == 0 || c != toupper(candidates[i][match_len]))
						all_candidates_matches = false;
				if (!all_candidates_matches)
					break;
				match_len++;
			}

			if (match_len > 0)
			{
				data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
				data->InsertChars(data->CursorPos, candidates[0].c_str(), candidates[0].c_str() + match_len);
			}

		}

		break;
	}
	case ImGuiInputTextFlags_CallbackHistory:
	{
		// Example of HISTORY
		const int prev_history_pos = commandHistoryIndex;
		if (data->EventKey == ImGuiKey_UpArrow)
		{
			if (commandHistoryIndex == -1)
				commandHistoryIndex = commandHistory.size() - 1;
			else if (commandHistoryIndex > 0)
				commandHistoryIndex--;
		}
		else if (data->EventKey == ImGuiKey_DownArrow)
		{
			if (commandHistoryIndex != -1)
				if (++commandHistoryIndex >= commandHistory.size())
					commandHistoryIndex = -1;
		}

		// A better implementation would preserve the data on the current input line along with cursor position.
		if (prev_history_pos != commandHistoryIndex)
		{
			data->CursorPos = data->SelectionStart = data->SelectionEnd = data->BufTextLen = (int)snprintf(data->Buf, (size_t)data->BufSize, "%s", (commandHistoryIndex >= 0) ? commandHistory[commandHistoryIndex].c_str() : "");
			data->BufDirty = true;
		}
	}
	}
	return 0;
}

void SE::Core::DevConsole::ExecuteCommand(std::string commandString)
{
	int argc = 0;
	char* argv[20] = {0};


	int j = 0;
	int i = 0;
	char c = commandString[i];
	while (c != '\0')
	{
		while (!(c == ' ' || c == '\0'))
		{
			i++;
			c = commandString[i];
		}
		argv[argc] = &commandString[j];
		argc++;
		if (commandString[i] != '\0')
		{
			commandString[i] = '\0';
			i++;
			j = i;
			c = commandString[i];
			if (argc >= 20)
			{
				break;
			}

		}
	}
	std::string name = std::string(argv[0]);
	auto find = nameToCommand.find(name);
	if (find == nameToCommand.end())
	{
		Print("Command: " + name + " not found.");
		return;
	}
	find->second.command(argc, argv);
}

