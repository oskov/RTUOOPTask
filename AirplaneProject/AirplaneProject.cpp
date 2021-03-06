#pragma once
#include "Headers.h"
#include "world.h"
#include "Windowsx.h"
#include "Database.h"

#define HEIGHT 800
#define WIDTH 1600

HINSTANCE hInst;                                
WCHAR windowTitle[]=L"Ariplanes";
WCHAR className[]=L"WindowClass"; 

WCHAR listTitle[] = L"Session Result";

HWND listWindow;
HWND mainWindow;

std::unique_ptr<World> world;

bool closed = false;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateListBoxWindow();

void RegisterWinClass(HINSTANCE hInstance)
{
	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = className;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	RegisterClass(&wc);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR cmd, int nCmdShow){
	#if defined _DEBUG
		AllocConsole();
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	#endif

	world = std::make_unique<World>();
	RegisterWinClass(hInstance);
	hInst = hInstance;
	mainWindow = CreateWindowEx(
		0,                              // Optional window styles.
		className,                     // Window class
		windowTitle,    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style
		0, 0, WIDTH, HEIGHT,// Size and position
		nullptr,       // Parent window    
		nullptr,       // Menu
		hInstance,  // Instance handle
		nullptr        // Additional application data
	);
	if (mainWindow == NULL)
	{	
		return 0;
	}
	ShowWindow(mainWindow, nCmdShow);

	MSG msg = {};
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			world->Tick();
		}
	}


	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE: 
		HRESULT hr;
		hr= world->Init(hwnd, WIDTH, HEIGHT);
		if (FAILED(hr)) {
			OutputDebugStringW(L"ERROR INIT");
			return -1;
		}
		
		return 0;
	case WM_DESTROY:

		PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		world->Exit();
		if (!closed&&MessageBox(hwnd, L"Show Results?", L"Results", MB_OKCANCEL) == IDOK)
		{
			closed = true;
			ShowWindow(mainWindow,SW_HIDE);
			CreateListBoxWindow();
			
		}else
		DestroyWindow(hwnd);
		return 0;
	case WM_LBUTTONUP: {
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		world->RemovePlane(x, y);
		return 0;
	}
	case WM_SIZE:
		world->OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
		return 0;
	case WM_PAINT:
	{
		world->Tick();
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hwnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(hwnd, &ps);
	}
	return 0;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void CreateListBoxWindow() {
	HWND window_handle = CreateWindowEx(WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE,
		className,
		L"Listbox Example",
		WS_OVERLAPPEDWINDOW |WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		100, 
		100, 
		WIDTH - 200,
		HEIGHT- 200, 
		NULL, NULL,
		hInst, 
		NULL);
	RECT client_rectangle;
	GetClientRect(window_handle, &client_rectangle);
	int width = client_rectangle.right - client_rectangle.left;
	int height = client_rectangle.bottom - client_rectangle.top;
	HWND listbox_handle = CreateWindowEx(WS_EX_CLIENTEDGE, 
		L"Listbox", 
		NULL,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL |LBS_DISABLENOSCROLL | LBS_NOSEL,

		10,
		10, 
		width - 20,
		height - 20,
		window_handle, 
		(HMENU)11,
		hInst, 
		NULL);
	vector<_bstr_t> data = world->GetSessionData();
	for (_bstr_t str : data) {
		SendMessage(listbox_handle, LB_ADDSTRING, 0, (LPARAM)str.GetBSTR());
	}

	ShowWindow(window_handle, SW_SHOW);

}