#include "Display.h"
#include "Input.h"
namespace SE {
	namespace Window {
		LRESULT CALLBACK Display::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
			ShowWindow(hWnd, SW_HIDE);
			DestroyWindow(hWnd);
			
		}

		bool Display::InitDisplay()
		{
			//returns a handle to the file used to create it (.exe file)
			HINSTANCE hInstance = GetModuleHandle(NULL);

			// If GetModuleHandle fails, there is no application handle for us to use returns false
			if (hInstance == NULL)
				return false;

			// this struct holds information for the window class
			WNDCLASSEX wc;

			// clear out the window class for use
			ZeroMemory(&wc, sizeof(WNDCLASSEX));

			// fill in the struct with the needed information
			wc.cbSize = sizeof(WNDCLASSEX);
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = WindowProc;
			wc.hInstance = hInstance;
			wc.hCursor = (HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
			wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
			wc.lpszClassName = "SE_DISPLAY";

			// register the window class
			if (!RegisterClassEx(&wc))
				return false;

			RECT rc = { 0, 0, width, height };
			AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

			// create the window and use the result as the handle
			hWnd = CreateWindowEx(NULL,
				"SE_DISPLAY",    // name of the window class
				"SE_DISPLAY",    // title of the window
				WS_OVERLAPPEDWINDOW,    // window style
				CW_USEDEFAULT,    // x-position of the window
				CW_USEDEFAULT,    // y-position of the window
				rc.right - rc.left,
				rc.bottom - rc.top,
				NULL,    // we have no parent window, NULL
				NULL,    // we aren't using menus, NULL
				hInstance,    // application handle
				NULL);    // used with multiple windows, NULL

			ShowWindow(hWnd, SW_SHOWDEFAULT);
			ShowCursor(true);
			return true;
		}

		void* Display::GethWnd()
		{
			return hWnd;
		}
	}	//namespace Window
}	//namespace SE
