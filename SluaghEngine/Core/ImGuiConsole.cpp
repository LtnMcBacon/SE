#include "ImGuiConsole.h"
#include <algorithm>
#include <Imgui\imgui.h>
#include <ImGuiDX11SDL\ImGuiDX11SDL.h>
#include <Profiler.h>

SE::Core::ImGuiConsole::ImGuiConsole(SE::Graphics::IRenderer* renderer, SE::Window::IWindow* window) : renderer(renderer), window(window)
{
	_ASSERT(renderer);
	_ASSERT(window);
	showConsole = false;
	inputBuffer.resize(256);
	commandHistoryIndex = -1;

	commands.AddCommand(
		[this](auto backend, int argc, char** argv)
	{
		Clear();
	},
		"clear",
		"Clears the console"
		);
}

SE::Core::ImGuiConsole::~ImGuiConsole()
{
}


void SE::Core::ImGuiConsole::Show()
{
	showConsole = true;
}

void SE::Core::ImGuiConsole::Hide()
{
	showConsole = false;
}

void SE::Core::ImGuiConsole::Toggle()
{
	showConsole = !showConsole;
}

bool SE::Core::ImGuiConsole::IsVisible()
{
	return showConsole;
}

void SE::Core::ImGuiConsole::BeginFrame()
{
	ImGuiDX11SDL_NewFrame();
}

void SE::Core::ImGuiConsole::Frame()
{
	StartProfile;
	if (!showConsole)
		ProfileReturnVoid;
	msgLock.lock();
	if(messages.size() == maxMessages)
	{
		//Keep half of the existing messages.
		messages.erase(messages.begin(), messages.begin() + (maxMessages / 2));
	}
	msgLock.unlock();
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Dev console", &showConsole, ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		ProfileReturnVoid;
	}
	

	for (auto& fc : frameCallbacks)
		fc();

	
	filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
	ImGui::Separator();
	
	ImGui::Checkbox("Scroll to bottom", &scrollToBottom);
	ImGui::Separator();
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
	msgLock.lock();
	for (auto& m : messages)
	{
		ImVec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		std::string lowerCaseChannel = m.channel;
		std::transform(lowerCaseChannel.begin(), lowerCaseChannel.end(), lowerCaseChannel.begin(), ::tolower);
		if (lowerCaseChannel.find("error") != std::string::npos)
			color = { 1.0f, 0.2f, 0.2f, 1.0f };
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		if(filter.IsActive())
		{
			if(filter.PassFilter(m.message.c_str()) || filter.PassFilter(m.channel.c_str()))
			{
				ImGui::TextUnformatted((m.channel + ": " + m.message).c_str());
			}
		}
		else
		{
			ImGui::TextUnformatted((m.channel + ": " + m.message).c_str());
		}	
		ImGui::PopStyleColor();
	}
	msgLock.unlock();
	if (scrollToBottom)
		ImGui::SetScrollHere();
	ImGui::EndChild();
	ImGui::Separator();
	if (ImGui::InputText("Input", inputBuffer.data(), inputBuffer.size(), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub, (void*)this))
	{
		char* input_end = inputBuffer.data() + strlen(inputBuffer.data());
		while (input_end > inputBuffer.data() && input_end[-1] == ' ') { input_end--; } *input_end = 0;
		if (inputBuffer[0])
		{
			std::string commandstring = std::string(inputBuffer.data(), input_end - inputBuffer.data());
			commands.InterpretCommand(this, commandstring.c_str());
			commandHistory.push_back(commandstring);
		}
		inputBuffer[0] = '\0';
	}

	// Demonstrate keeping auto focus on the input box
	if (ImGui::IsItemHovered() || (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
		ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
	ImGui::End();
	
	ProfileReturnVoid;
}

void SE::Core::ImGuiConsole::EndFrame()
{
	ImGui::Render();
}

void SE::Core::ImGuiConsole::Clear()
{
	msgLock.lock();
	messages.clear();
	msgLock.unlock();
}

void * SE::Core::ImGuiConsole::GetContext()
{
	return ImGui::GetCurrentContext();
}

void SE::Core::ImGuiConsole::AddFrameCallback(const std::function<void()>& frameCallback)
{
	frameCallbacks.push_back(frameCallback);
}

int SE::Core::ImGuiConsole::RemoveCommand(const char* name)
{
	commands.RemoveCommand(name);
	return 0;
}

int SE::Core::ImGuiConsole::TextEditCallbackStub(ImGuiTextEditCallbackData * data)
{
	ImGuiConsole* devcon = (ImGuiConsole*)data->UserData;
	return devcon->TextEditCallback(data);
}

int SE::Core::ImGuiConsole::TextEditCallback(ImGuiTextEditCallbackData * data)
{

	StartProfile;
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
		std::map<size_t, DevConsole::Commands::Command_Structure> commandmap;
		commands.GetMap(commandmap);
		for (auto& c : commandmap)
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


	ProfileReturnConst( 0);
}

int SE::Core::ImGuiConsole::Initialize()
{
	if(!ImGuiDX11SDL_Init(renderer, window))
		return false;
	return true;
}

void SE::Core::ImGuiConsole::Shutdown()
{
	ImGuiDX11SDL_Shutdown();
}

int SE::Core::ImGuiConsole::AddCommand(const DevConsole::DevConsole_Command & commandFunction, char * name, char * description)
{
	return commands.AddCommand(commandFunction, name, description);
}

#include <stdarg.h>  // For va_start, etc.

void SE::Core::ImGuiConsole::PrintChannel(const char * channel, const char * line, ...)
{
	std::string fmt_str = line;
	size_t size = fmt_str.size() * 2;
	char* formatted = new char[size];
	va_list args;
	va_start(args, line);

	auto r = vsnprintf(formatted, size, line, args);
	while (r < 0 || r > size)
	{
		delete[] formatted;
		size *= 2;
		formatted = new char[size];
		r = vsnprintf(formatted, size, line, args);
	}

	va_end(args);
	std::string f(formatted, r);
	msgLock.lock();
	messages.push_back({ std::chrono::system_clock::now(), f, channel });
	msgLock.unlock();
	delete[] formatted;
}

void SE::Core::ImGuiConsole::Print(const char * line, ...)
{
	std::string fmt_str = line;
	size_t size = fmt_str.size()*2;
	char* formatted = new char[size];
	va_list args;
	va_start(args, line);

	auto r = vsnprintf(formatted, size, line, args);
	while (r < 0 || r > size)
	{
		delete[] formatted;
		size *= 2;
		formatted = new char[size];
		r = vsnprintf(formatted, size, line, args);
	}

	va_end(args);
	std::string f(formatted, r);
	msgLock.lock();
	messages.push_back({ std::chrono::system_clock::now(), f, "Global" });
	msgLock.unlock();
	delete[] formatted;
}

void SE::Core::ImGuiConsole::VPrint(const char * line, va_list args)
{
	std::string fmt_str = line;
	size_t size = fmt_str.size() * 2;
	char* formatted = new char[size];

	auto r = vsnprintf(formatted, size, line, args);
	while (r < 0 || r > size)
	{
		delete[] formatted;
		size *= 2;
		formatted = new char[size];
		r = vsnprintf(formatted, size, line, args);
	}

	std::string f(formatted, r);
	msgLock.lock();
	messages.push_back({ std::chrono::system_clock::now(), f, "Global" });
	msgLock.unlock();
	delete[] formatted;
}

void SE::Core::ImGuiConsole::VPrint(const char * channel, const char * line, va_list args)
{
	std::string fmt_str = line;
	size_t size = fmt_str.size() * 2;
	char* formatted = new char[size];

	auto r = vsnprintf(formatted, size, line, args);
	while (r < 0 || r > size)
	{
		delete[] formatted;
		size *= 2;
		formatted = new char[size];
		r = vsnprintf(formatted, size, line, args);
	}

	std::string f(formatted, r);
	msgLock.lock();
	messages.push_back({ std::chrono::system_clock::now(), f, channel });
	msgLock.unlock();
	delete[] formatted;
}

void SE::Core::ImGuiConsole::Getline(std::string & string)
{
}

size_t SE::Core::ImGuiConsole::Getline(const char * buffer, size_t size)
{
	return size_t();
}
