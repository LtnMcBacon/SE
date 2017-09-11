#include "Display.h"

namespace SE {
	namespace Window {
		LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			// sort through and find what code to run for the message given
			switch (message)
			{
				// this message is read when the window is closed
			case WM_DESTROY:
			{
				// close the application entirely
				PostQuitMessage(0);
				return 0;
			} break;
			}

			// Handle any messages the switch statement didn't
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		Display::Display()
		{

		}

		Display::~Display()
		{

		}

		bool Display::InitDisplay()
		{
			//returns a handle to the file used to create it (.exe file)
			HINSTANCE l_hInstance = GetModuleHandle(NULL);

			// this struct holds information for the window class
			WNDCLASSEX l_wc;

			// clear out the window class for use
			ZeroMemory(&l_wc, sizeof(WNDCLASSEX));

			// fill in the struct with the needed information
			l_wc.cbSize = sizeof(WNDCLASSEX);
			l_wc.style = CS_HREDRAW | CS_VREDRAW;
			l_wc.lpfnWndProc = WindowProc;
			l_wc.hInstance = l_hInstance;
			l_wc.hCursor = (HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
			l_wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
			l_wc.lpszClassName = "SE_DISPLAY";

			// register the window class
			if (!RegisterClassEx(&l_wc))
				return false;

			RECT l_rc = { 0, 0, m_width, m_height };
			AdjustWindowRect(&l_rc, WS_OVERLAPPEDWINDOW, FALSE);

			// create the window and use the result as the handle
			m_hWnd = CreateWindowEx(NULL,
				"SE_DISPLAY",    // name of the window class
				"SE_DISPLAY",    // title of the window
				WS_OVERLAPPEDWINDOW,    // window style
				CW_USEDEFAULT,    // x-position of the window
				CW_USEDEFAULT,    // y-position of the window
				l_rc.right - l_rc.left,
				l_rc.bottom - l_rc.top,
				NULL,    // we have no parent window, NULL
				NULL,    // we aren't using menus, NULL
				l_hInstance,    // application handle
				NULL);    // used with multiple windows, NULL

			ShowWindow(m_hWnd, 1);
			ShowCursor(true);
			return true;
		}

		const HWND& Display::GethWnd()
		{
			return m_hWnd;
		}
	}	//namespace Window
}	//namespace SE
