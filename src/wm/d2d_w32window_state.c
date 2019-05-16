#include "d2d_w32window_state.h"
#include <stdio.h>
#include <stdlib.h>

// NOTE: UNFINISHED, I was/am having issues working with direct2d, not at the same level as the X11/GDI versions

// Unsure if I should generate this anew (randomly or dev supplied)
// or just use this.
const char g_szClassName[] = "M_WINDOWCLASS";

WINDOW_STATE* construct_window_state_simple (int width, int height) {
	WINDOW_STATE_OPTIONS opts = window_state_options_defaults();
	opts.width = width;
	opts.height = height;
	return construct_window_state(opts);
}

#define ID_FILE_EXIT 9001
#define ID_STUFF_GO 9002
#define ID_HELP_ABOUT 9003
#define IDC_MAIN_EDIT 9004


/* TODO:
https://docs.microsoft.com/en-us/windows/desktop/direct2d/getting-started-with-direct2d#step-1-include-direct2d-header
	When you create a render target and hardware acceleration is available, you allocate resources on the computer's GPU. 
	By creating a render target once and retaining it as long as possible, you gain performance benefits. Your application 
	should create render targets once and hold on to them for the life of the application or until the D2DERR_RECREATE_TARGET 
	error is received. When you receive this error, you need to recreate the render target (and any resources it created).
*/
// NOTE: The docs say to use SafeRelease, but I use the specific versions since I don't have any 'generic' version




LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		/*GDI drawing
		case WM_PAINT:
		{

			PAINTSTRUCT ps;
			BeginPaint(hwnd, &ps);

			// Obtain the size of the drawing area.
			RECT rc;
			GetClientRect(hwnd, &rc);

			// Save the original object
			HGDIOBJ original = NULL;
			original = SelectObject(ps.hdc, GetStockObject(DC_PEN));

			// Create a pen.
			HPEN blackPen = CreatePen(PS_SOLID, 3, 0);

			// Select the pen.
			SelectObject(ps.hdc, blackPen);

			// Draw a rectangle.
			Rectangle(
				ps.hdc,
				rc.left + 100,
				rc.top + 100,
				rc.right - 100,
				rc.bottom - 100
			);

			DeleteObject(blackPen);

			// Restore the original object
			SelectObject(ps.hdc, original);

			EndPaint(hwnd, &ps);
		}
		return 0;*/
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		// Places WM_QUIT msg into the queue.
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
	{ // <- required for declaring variables in a switch statement.
		char szFileName[MAX_PATH];
		HINSTANCE hInstance = GetModuleHandle(NULL);
		GetModuleFileName(hInstance, szFileName, MAX_PATH);

		MessageBox(hwnd, szFileName, "This program is:", MB_OK | MB_ICONINFORMATION);
	}
	break;
	case WM_CREATE:
	{
		/*
		HMENU hMenu, hSubMenu;
		HICON hIcon, hIconSm;

		hMenu = CreateMenu();

		hSubMenu = CreatePopupMenu();
		AppendMenu(hSubMenu, MF_STRING, ID_FILE_EXIT, "E&xit");
		AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&File");

		hSubMenu = CreatePopupMenu();
		AppendMenu(hSubMenu, MF_STRING, ID_STUFF_GO, "&Go");
		AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&Stuff");

		SetMenu(hwnd, hMenu);

		hIcon = LoadImage(NULL, "resources/menu_two.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
		if (hIcon) {
			SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		} else {
			MessageBox(hwnd, "Could not load large icon!", "Error", MB_OK | MB_ICONERROR);
		}

		hIconSm = LoadImage(NULL, "resources/menu_two.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
		if (hIconSm) {
			SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSm);
		} else {
			MessageBox(hwnd, "Could not load small icon!", "Error", MB_OK | MB_ICONERROR);
		}


		// Textbox
		HWND hEdit;

		hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
			0, 0, 100, 100, // x,y,w,h,
			hwnd, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL
		);

		if (hEdit == NULL) {
			MessageBox(hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);
		}

		//HFONT hfDefault = GetStockObject(DEFAULT_GUI_FONT);
		// this is how msdn wants you to do it
		NONCLIENTMETRICS ncm;
		ncm.cbSize = sizeof(NONCLIENTMETRICS);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
		LOGFONT lfMessageFont = ncm.lfMessageFont;
		HFONT hfDefault = CreateFontIndirect(&lfMessageFont);

		SendMessage(hEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
		*/
	}
	break;
	/*case WM_SIZE:
	{
		HWND hEdit;
		RECT rcClient;

		GetClientRect(hwnd, &rcClient);

		hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
		SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);

	}
	break;*/
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
			case ID_FILE_EXIT:
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
			case ID_STUFF_GO:
				printf("Hi!");
			break;
			case ID_HELP_ABOUT:
			{
				//CreateDialog()
				//int ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT))
			}
			break;
		}
	break;
	break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}


WINDOW_STATE* construct_window_state (WINDOW_STATE_OPTIONS opts) {
	WINDOW_STATE* ws = (WINDOW_STATE*) calloc(1, sizeof(WINDOW_STATE));
	if (ws == NULL) {
		free(ws);
		return NULL;
	}

	dlist_init(&(ws->dlist));

	ws->wc.cbSize = sizeof(WNDCLASSEX);
	ws->wc.style = 0;
	ws->wc.lpfnWndProc = WndProc;
	//Amount of extra data allocated in mem. Usually 0
	ws->wc.cbClsExtra = 0;
	// Amount of extra data allocated in mem per window of this type. Usually 0.
	ws->wc.cbWndExtra = 0;
	// Normally this is given from WinMain, may have to do that ?
	ws->wc.hInstance = GetModuleHandle(NULL);
	// 32x32, shown when user uses alt+tab
	ws->wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	ws->wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	ws->wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	ws->wc.lpszMenuName = NULL;
	ws->wc.lpszClassName = g_szClassName;
	// 16x16 icon to show in the askbar and in the topleft corner of the window
	ws->wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	// TODO: add general option for if it should show errors about window creation
	if (!RegisterClassEx(&(ws->wc))) {
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		free(ws);
		return NULL;
	}

	// TODO: should I free or mark the ws->wc as dead (or whatever it is), because RegisterClassEx might hold it somewhere?
	ws->hwnd = CreateWindowEx(
		// Extended windows style
		WS_EX_CLIENTEDGE,
		g_szClassName,
		// titlebar text
		"W32 Window",
		WS_OVERLAPPEDWINDOW,
		opts.x, opts.y,
		opts.width, opts.height,
		NULL, NULL, NULL, NULL
	);

	if (ws->hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		free(ws);
		// Unsure if I need to do anything with ws->hwnd, or ws->wc
		return NULL;
	}

	// TODO: make an option for if it should be true
	ShowWindow(ws->hwnd, 1);
	UpdateWindow(ws->hwnd);

	HRESULT factoryCreationResult = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&IID_ID2D1Factory, // i think this is right, docs aren't clear
		&((D2D1_FACTORY_OPTIONS) {
			.debugLevel=D2D1_DEBUG_LEVEL_INFORMATION
		}),
		(void**) &(ws->d2dFactory)
	);

	printf("factoryCreationResult: %ld\n", factoryCreationResult);
	if (!SUCCEEDED(factoryCreationResult)) {
		MessageBox(NULL, "Direct2d Factory Creation Failed", "Error!", MB_ICONEXCLAMATION | MB_OK);
		free(ws);
		return NULL;
	}

	RECT rc;
	GetClientRect(ws->hwnd, &rc);
	printf("rc: (%ld, %ld) %ldx%ld\n", rc.left, rc.top, rc.right, rc.bottom);
	HRESULT hwndRenderTargetCreationResult = ID2D1Factory_CreateHwndRenderTarget(
		ws->d2dFactory,
		&((D2D1_RENDER_TARGET_PROPERTIES) {
			.type = D2D1_RENDER_TARGET_TYPE_DEFAULT,
			.pixelFormat = (D2D1_PIXEL_FORMAT) {
    			.format = DXGI_FORMAT_B8G8R8A8_UNORM,
    			.alphaMode = D2D1_ALPHA_MODE_IGNORE
			},
			.dpiX = 0, // defaults
			.dpiY = 0, // defaults
			// TODO: look further at https://docs.microsoft.com/en-us/windows/desktop/api/d2d1/ne-d2d1-d2d1_render_target_usage
			.usage = D2D1_RENDER_TARGET_USAGE_NONE,
			.minLevel = D2D1_FEATURE_LEVEL_DEFAULT
		}),
		&((D2D1_HWND_RENDER_TARGET_PROPERTIES) {
			.hwnd = ws->hwnd,
			// TODO: look at the properties for this, and the C++ method that creates it
			.pixelSize = (D2D1_SIZE_U) {
				// https://docs.microsoft.com/en-us/windows/desktop/api/d2d1helper/nf-d2d1helper-hwndrendertargetproperties
				// says this is the default
				.width = 0,
				.height = 0,
			},
			/*(D2D1_SIZE_U) {
				.width = rc.right - rc.left,
				.height = rc.bottom - rc.top
			},*/
			.presentOptions = D2D1_PRESENT_OPTIONS_NONE
		}),
		&(ws->d2dRenderTarget)
	);

	printf("hwndRenderTargetCreationResult: %ld\n", hwndRenderTargetCreationResult);

	if (!SUCCEEDED(hwndRenderTargetCreationResult)) {
		MessageBox(NULL, "Direct2d Creating HWND Render Target Failed", "Error!", MB_ICONEXCLAMATION | MB_OK);
		ID2D1Factory_Release(ws->d2dFactory);
		// TODO: Don't know if I need to Release the hwnd-render-target if it's freed next and it failed being created
		free(ws);
		return NULL;
	}
	printf("Beep\n");

	ws->b_col.r = 1.0f;
	ws->b_col.g = 0.5f;
	ws->b_col.b = 0.0f;
	ws->b_col.a = 1.0f;
	printf("b_col done\n");

	ws->brushProp.opacity = 1.0f;
	printf("Opacity done\n");
	ws->brushProp.transform._11 = 1.0f;
	ws->brushProp.transform._12 = 0.0f;
	ws->brushProp.transform._21 = 0.0f;
	ws->brushProp.transform._22 = 1.0f;
	ws->brushProp.transform._31 = 0.0f;
	ws->brushProp.transform._32 = 0.0f;
	printf("Creating solid color brush\n");
	ID2D1HwndRenderTarget_CreateSolidColorBrush(
		ws->d2dRenderTarget,
		&(ws->b_col),
		/*
		&((D2D1_BRUSH_PROPERTIES) {
			// 0-1
			.opacity = 1.0f,
			.transform = (D2D1_MATRIX_3X2_F) {
				._11 = 1.0f,
				._12 = 0.0f,
				._21 = 0.0f,
				._22 = 1.0f,
				._31 = 0.0f,
				._32 = 0.0f
			}
		}),*/
		&(ws->brushProp),
		&(ws->pBlackBrush)
	);
	printf("Created brush\n");
	ID2D1HwndRenderTarget_BeginDraw(ws->d2dRenderTarget);
	printf("Drawing\n");

	//D2D1_RECT_F* drawingRect = (D2D1_RECT_F*) calloc(1, sizeof(D2D1_RECT_F));
	ws->rect.left = 5;
	ws->rect.right = 10;
	ws->rect.top = 10;
	ws->rect.bottom = 20;
	/*(D2D1_RECT_F) {
		.left = 5,
		.right = 10,
		.bottom = 20,
		.top = 10
	};*/

	printf("ws->rect: (%f, %f) %fx%f\n", ws->rect.left, ws->rect.top, ws->rect.right, ws->rect.bottom);

	ID2D1HwndRenderTarget_DrawRectangle(
		ws->d2dRenderTarget, // Render target
		&(ws->rect), // &rect
		(ID2D1Brush*)ws->pBlackBrush, // brush
		2.0f, // strokewidth
		NULL // stroke style, solid stroke
	);
	printf("ws->rect: (%f, %f) %fx%f\n", ws->rect.left, ws->rect.top, ws->rect.right, ws->rect.bottom);
	ID2D1HwndRenderTarget_FillRectangle(
		ws->d2dRenderTarget,
		&(ws->rect),
		(ID2D1Brush*)ws->pBlackBrush
	);
	printf("ws->rect: (%f, %f) %fx%f\n", ws->rect.left, ws->rect.top, ws->rect.right, ws->rect.bottom);
	D2D1_COLOR_F color = ws->pBlackBrush->lpVtbl->GetColor(ws->pBlackBrush);

	printf("color: %f %f %f %f\n", color.r, color.g, color.b, color.a);


	HRESULT drawingResult = ID2D1HwndRenderTarget_EndDraw(
		ws->d2dRenderTarget,
		NULL, NULL
	);

	printf("drawingResult: %ld\n", drawingResult);
	if (!SUCCEEDED(drawingResult)) {
		printf("Failed to draw %d\n", drawingResult);
	}


	return ws;
}

void free_window_state (WINDOW_STATE* wstate) {
	// TODO: do I need to free the dlist items
	// TODO: do  I need to free the Direct2d stuff
	// Note: Direct2d docs say to use SafeRelease, but that's a C++ thing and Idk a good way to define it
	// that will work fine for every situation in C, so we try to use their specific versions, and hope they don't cause issues
	ID2D1Factory_Release(wstate->d2dFactory);
	ID2D1HwndRenderTarget_Release(wstate->d2dRenderTarget);
	ID2D1SolidColorBrush_Release(wstate->pBlackBrush);

	free(wstate);
}
void print_window_state (WINDOW_STATE* ws) {
	printf(
		"WINDOW_STATE (w32) {\n\t"
			"dlist: %d size %d capacity,\n"
		"}\n",
		dlist_length(&(ws->dlist)),
		ws->dlist.capacity
	);
}

// Note: in_ stands for internal
// I use it because there might be a function that returns dimensions someday
// and I don't want to use the name.
RECT in_get_window_state_dimensions (WINDOW_STATE* ws) {
	RECT dimensions;
	GetWindowRect(ws->hwnd, &dimensions);

	return dimensions;
}

// TODO: perhaps cast these to (long)'s? it uses win32 'LONG'
long get_window_state_width (WINDOW_STATE* ws) {
	return in_get_window_state_dimensions(ws).right;
}
long get_window_state_height (WINDOW_STATE* ws) {
	return in_get_window_state_dimensions(ws).bottom;
}
long get_window_state_xpos (WINDOW_STATE* ws) {
	return in_get_window_state_dimensions(ws).left;
}
long get_window_state_ypos (WINDOW_STATE* ws) {
	return in_get_window_state_dimensions(ws).top;
}

DrawerList* get_window_state_dlist (WINDOW_STATE* wstate) {
	return &(wstate->dlist);
}
void add_drawer_to_window_state (WINDOW_STATE* wstate, Drawer drawer) {
	dlist_append(get_window_state_dlist(wstate), allocate_drawer(drawer));
}



M_LRect mlrect_from_w32RECT (RECT rect) {
	// TODO: Make sure this works properly
	return (M_LRect) {
		.x = rect.left,
		.y = rect.top,
		.width = rect.right,
		.height = rect.bottom
	};
}