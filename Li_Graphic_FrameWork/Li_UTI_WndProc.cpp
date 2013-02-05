#include "Li_UTI_FrameWork.h"

  ////////////////////////////
 // The Windows Procedures //
////////////////////////////
LRESULT CALLBACK Li_FW::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_COMMAND:
			{
			} break;

		case WM_MOVE:
			{
				fn_UpdateFrame();
			} break;

		case WM_SIZE:
			{
				if (wParam != SIZE_MINIMIZED)
				{
					m_WinWidth = LOWORD (lParam);
					m_WinHeight = HIWORD (lParam);
					fn_WindowResize();
					fn_UpdateFrame();
				}
			} break;

		case WM_SETCURSOR:
			{
				// Hide Cursor in Client Rectangle but not on Title Bar
				 WORD ht = LOWORD(lParam);
				static bool hiddencursor = false;
				if (HTCLIENT==ht && !hiddencursor)
				{
					hiddencursor = true;
					ShowCursor(false);
				}
				else if (HTCLIENT!=ht && hiddencursor) 
				{
					hiddencursor = false;
					ShowCursor(true);
				}
			} break;

		case WM_MOUSEMOVE:
			{
				// this is for the old winapi mouse, now I updated to directx input
				s_MousePosAbs.x = LOWORD (lParam);
				s_MousePosAbs.y = HIWORD (lParam);

				fn_UpdateFrame();
			} break;

		case WM_LBUTTONDOWN:
			{
				s_isLBtnDown = true;
				s_isLClick	 = true;
				fn_UpdateFrame();
			} break;

		case WM_LBUTTONUP:
			{
				s_isLBtnDown = false;
				s_isLRelease = true;
				fn_UpdateFrame();
			} break;

		case WM_RBUTTONDOWN:
			{
				s_isRBtnDown = true;
				s_isRClick	 = true;
				fn_UpdateFrame();
			} break;

		case WM_RBUTTONUP:
			{
				s_isRBtnDown = false;
				s_isRRelease = true;
				fn_UpdateFrame();
			} break;

		case WM_DESTROY:
			{
				// kill the application, this sends a WM_QUIT message
				PostQuitMessage(0);

				return 0;
			} break;

		default:
			{
				s_isLClick	 = false;
				s_isRClick	 = false;
				s_isLRelease = false;
				s_isRRelease = false;
			} break;
	}

	// process any messages that we didn't take care of 
	return (DefWindowProc(hWnd, msg, wParam, lParam));
}
// End Windows Procedures ////