#ifndef FILE_M_W32GDIWINDOWSTATE
#define FILE_M_W32GDIWINDOWSTATE

#include <windows.h>

#include "window_state.h"
#include "../mutil.h"
#include "drawer.h"

typedef struct WINDOW_STATE_DATA_t {
	DrawerList dlist;
	bool should_end;

	// Windows parts
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	HDC hdc;
	PAINTSTRUCT ps;

	HPEN defaultPen;
	HBRUSH defaultBrush;

	unsigned long event_flags;
	MWS_EVENT_LIST events;
} WINDOW_STATE;

typedef struct WINDOW_STATE_RAW_EVENT_t {
	UINT Message;
	WPARAM wParam;
	LPARAM lParam;
} WINDOW_STATE_RAW_EVENT;

MWS_EVENT create_full_mwsevent (MWS_EVENT_TYPE type, UINT Message, WPARAM wParam, LPARAM lParam);

#endif