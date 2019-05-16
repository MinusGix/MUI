#ifndef FILE_M_W32D2DWINDOWSTATE
#define FILE_M_W32D2DWINDOWSTATE

#include <windows.h>
#include <initguid.h>
//#include <commoncontrols.h>
//#include <mmdeviceapi.h>
//#include <endpointvolume.h>
#include <d2d1.h>

#include "window_state.h"
#include "../mutil.h"
#include "drawer.h"
#include  <objidl.h>
#pragma comment(lib, "d2d1.lib")

typedef struct WINDOW_STATE_DATA_t {
	DrawerList dlist;

	// Windows parts

	// See: https://docs.microsoft.com/en-us/windows/desktop/api/winuser/ns-winuser-tagwndclassexa
	WNDCLASSEX wc;
	HWND hwnd;

	ID2D1Factory* d2dFactory;
	ID2D1HwndRenderTarget* d2dRenderTarget;
	ID2D1SolidColorBrush* pBlackBrush;

	// just testing, remove these later
	D2D1_RECT_F rect;
	D2D1_COLOR_F b_col;
	D2D1_BRUSH_PROPERTIES brushProp;
} WINDOW_STATE;

M_LRect mlrect_from_w32RECT (RECT rect);

#endif