#pragma comment( lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include <stdlib.h>
#include <string>
#include "resources.h"

LRESULT CALLBACK WinProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst,LPSTR lpCmdLine,int nShowCmd)
{
	WNDCLASSEX winClass;

	ZeroMemory(&winClass,sizeof(WNDCLASSEX));
	winClass.cbClsExtra=NULL;
	winClass.cbSize=sizeof(WNDCLASSEX);
	winClass.cbWndExtra=NULL;
	winClass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	winClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	winClass.hIcon=LoadIcon (NULL, IDI_APPLICATION);
	winClass.hIconSm=LoadIcon (NULL, IDI_APPLICATION);
	winClass.hInstance=hInstance;
	winClass.lpfnWndProc=(WNDPROC)WinProc;
	winClass.lpszClassName="Window Class";
	winClass.lpszMenuName=MAKEINTRESOURCE(Menu); //using menu form rc file
	winClass.style=CS_HREDRAW|CS_VREDRAW;

	if(!RegisterClassEx(&winClass))
	{
		int nResult=GetLastError();
		MessageBox(NULL,
			"Window class creation failed",
			"Window Class Failed",
			MB_ICONERROR);
	}

	//receive screen dimensions
	int screenHeight = GetSystemMetrics(SM_CYSCREEN); 
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);

	

	HWND hWnd=CreateWindowEx(NULL,
			"Window Class",
			"Lab#2",
			WS_OVERLAPPEDWINDOW| 
			WS_HSCROLL | WS_VSCROLL,
			screenWidth/6,		//leftx
			screenHeight/7,		//lefty
			screenWidth - 2*screenWidth/6, //rightx
			screenHeight - 2*screenHeight/7, //righty
			NULL,
			NULL,
			hInstance,
			NULL);
	
	if(!hWnd)
	{
		int nResult=GetLastError();
		MessageBox(NULL,
			"Window creation failed",
			"Window Creation Failed",
			MB_ICONERROR);
	}
	
	//registering hotkey
	
	RegisterHotKey(hWnd, IDH_BGCHANGER, MOD_CONTROL, VK_SPACE);
	RegisterHotKey(hWnd, IDH_EXITHOTKEY, MOD_CONTROL, 0x4D);
	
	
	ShowWindow(hWnd,nShowCmd);

	MSG msg;
	ZeroMemory(&msg,sizeof(MSG));

	while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

LRESULT CALLBACK WinProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	static int  iColorID[5] = { 
		WHITE_BRUSH,  LTGRAY_BRUSH, GRAY_BRUSH,
		DKGRAY_BRUSH, BLACK_BRUSH };
    static int  iSelection = IDM_WHITE ;
    static HMENU  hMenu;

	static HWND hEdit;

	static int addX=0; // 0 is initial
	static int addY=0; // 0 is initial
	switch(msg)
	{

		//minmaxinfo - setting limits on window dimensions
		case WM_GETMINMAXINFO:
		{
			int screenHeight = GetSystemMetrics(SM_CYSCREEN);
			int screenWidth = GetSystemMetrics(SM_CXSCREEN);
			MINMAXINFO* mmi = (MINMAXINFO*)lParam;
			mmi->ptMinTrackSize.x = 3*screenWidth/6;
			mmi->ptMinTrackSize.y = 3*screenHeight/7;
			mmi->ptMaxTrackSize.x = screenWidth - 2*screenWidth/6;
			mmi->ptMaxTrackSize.y = screenHeight - 2*screenHeight/7;
		}break;
		//end getminmaxinfo

		case WM_CREATE:
		{
			SetScrollRange(hWnd, SB_VERT, 0, 100, FALSE);
			hEdit = CreateWindowEx(WS_EX_CLIENTEDGE,
				"EDIT",
				"",
				WS_CHILD|WS_VISIBLE|
				WS_EX_CLIENTEDGE|ES_MULTILINE| WS_VSCROLL,
				50,
				100,
				200,
				100,
				hWnd,
				(HMENU)IDC_EDIT,
				GetModuleHandle(NULL),
				NULL);

			HGDIOBJ hfDefault=GetStockObject(2);
			SendMessage(hEdit,
				WM_SETFONT,
				(WPARAM)hfDefault,
				MAKELPARAM(FALSE,0));
			SendMessage(hEdit,
				WM_SETTEXT,
				NULL,
				(LPARAM)"Insert text here...");


		}		break;
		//end wm_create

		case WM_VSCROLL:
		{
			switch(LOWORD(wParam))
			{
			case SB_THUMBPOSITION:
				{
					addY+=10;
					SendMessage(hWnd, WM_SIZE, 0L, 0L);
					SetScrollPos(hWnd, SB_VERT, 50, FALSE); 
				}break;
			}
		}break;
		//end wm_vscroll
		case WM_HSCROLL:
		{
			switch(LOWORD(wParam))
			{
				case SB_THUMBPOSITION:
					{
						addX+=10;
						SendMessage(hWnd, WM_SIZE, 0L, 0L);
						SetScrollPos(hWnd, SB_HORZ, 50, FALSE); 
					}break;
			}
		}break;



		case WM_HOTKEY:
			{
				switch(LOWORD(wParam))
				{
				case IDH_BGCHANGER:
					{
						SendMessage(hWnd, WM_COMMAND, (WPARAM)IDM_RANDOM, NULL);
					}break;
				case IDH_EXITHOTKEY:
					{
						SendMessage(hWnd, WM_DESTROY, NULL, NULL);
					}break;
				}
			}break;
		//end wm_hotkey

		case WM_COMMAND:
			{
				hMenu = GetMenu(hWnd) ;
				static int red, green, blue;
				red = rand()%255;
				blue = rand()%255;
				green = rand()%255;
				HBRUSH brush = CreateSolidBrush(RGB(red, green, blue));

				switch (LOWORD(wParam))
					{
					case IDM_RESTART:
						{
							red = green = blue = 255;
							brush = CreateSolidBrush(RGB(red, green, blue));
							CheckMenuItem (hMenu, iSelection, MF_UNCHECKED) ;
							iSelection = IDM_WHITE ;				// restarts BgColor menu to White
							CheckMenuItem (hMenu, iSelection, MF_CHECKED) ;
							SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, LONG(brush));
							InvalidateRect (hWnd, NULL, TRUE) ;
							MessageBeep (0) ;
							addX = addY = 0;
							SendMessage(hWnd, WM_SIZE, 0L, 0L);
						}break;

					case IDM_EXIT:
						{
							SendMessage (hWnd, WM_CLOSE, 0, 0L) ;
							return 0 ;
						}
					case IDM_RANDOM:
						{
							CheckMenuItem (hMenu, iSelection, MF_UNCHECKED) ;
							iSelection = LOWORD (wParam) ;
							CheckMenuItem (hMenu, iSelection, MF_CHECKED) ;
							SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, LONG(brush));
							InvalidateRect (hWnd, NULL, TRUE) ;
							return 0 ;
						}break;
					case IDM_WHITE :          
					case IDM_LTGRAY :         
					case IDM_GRAY :           
					case IDM_DKGRAY :         
					case IDM_BLACK :         
							CheckMenuItem (hMenu, iSelection, MF_UNCHECKED) ;
							iSelection = LOWORD (wParam) ;
							CheckMenuItem (hMenu, iSelection, MF_CHECKED) ;
							SetClassLong (hWnd, GCL_HBRBACKGROUND,
								(LONG) GetStockObject 
									(iColorID[LOWORD (wParam) - IDM_WHITE])) ; //select color iColorID[index-1]  
							InvalidateRect (hWnd, NULL, TRUE) ;
							return 0 ;
					case IDM_HELP :
							MessageBox (hWnd, "CTRL+SPACE for changing background color\n CTRL+M",
								"Lab#2", MB_ICONEXCLAMATION | MB_OK) ;
							return 0 ;

					case IDM_ABOUT :
							MessageBox (hWnd, "Developed by @andrewcap",
								"Lab#2", MB_ICONINFORMATION | MB_OK) ;
							return 0 ;
				}
				//end switch()
			}break;
		//end wm_command


		case WM_PAINT:
		{

		}break;
		//end wm_paint

		case WM_SIZE:
        {
			RECT rcWind;
			GetWindowRect(hWnd, &rcWind);

			SetWindowPos(GetDlgItem(hWnd, IDC_EDIT), 
				HWND_TOP, 
				(rcWind.right-rcWind.left)/2 -100+addX,
				(rcWind.bottom-rcWind.top)/2-50+addY, 
				200, 
				100, 
				SWP_NOSIZE);
			
        }break;
		//end wm_size

		case WM_DESTROY:
		{
			UnregisterHotKey(hWnd, IDH_BGCHANGER);
			UnregisterHotKey(hWnd, IDH_EXITHOTKEY);
			PostQuitMessage(0);		
			return 0;
		}
		//end wm_destroy
	}
	return DefWindowProc(hWnd,msg,wParam,lParam);
}
