#include "gdi_w32window_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

WINDOW_STATE* construct_window_state_simple (int width, int height) {
	WINDOW_STATE_OPTIONS opts = window_state_options_defaults();
	opts.width = width;
	opts.height = height;
	return construct_window_state(opts);
}
void print_window_state (WINDOW_STATE* ws) {
	// TODO: add detail
	printf(
		"WINDOW_STATE (w32) {\n\t"
			"dlist: %d size %d capacity,\n"
		"}\n",
		dlist_length(&(ws->dlist)),
		ws->dlist.capacity
	);
}

const char g_szClassName[] = "myWindowClass";

void pre_draw_all (WINDOW_STATE* wstate) {
	wstate->hdc = BeginPaint(wstate->hwnd, &(wstate->ps));
}
void post_draw_all (WINDOW_STATE* wstate) {
	EndPaint(wstate->hwnd, &(wstate->ps));
}
bool draw_rectangle_drawer (WINDOW_STATE* wstate, RectangleDrawer* d) {
	if (d->should_fill) {
		SelectObject(wstate->hdc, wstate->defaultBrush);
		Rectangle(
			wstate->hdc,
			d->x,
			d->y,
			d->x + d->width,
			d->y + d->height
		);
	} else {
		SelectObject(wstate->hdc, wstate->defaultPen);
		Rectangle(
			wstate->hdc,
			d->x,
			d->y,
			d->x + d->width,
			d->y + d->height
		);
	}
	return true;
}
bool draw_string_drawer (WINDOW_STATE* wstate, StringDrawer *d) {
	TextOutA(wstate->hdc, d->x, d->y, d->str, d->length);
	return true;
}

// Note: good regex for searching for what a msg id is: 0x(0)*ID$
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	WINDOW_STATE *ws = (WINDOW_STATE*) GetWindowLongPtr(hwnd, GWLP_USERDATA);

	if (ws == NULL) {
		if (Message == WM_NCCREATE) {
			printf("[INFO]WndProc WM_NCCREATE\n");
			// Retrieve WINDOW_STATE instance from window creation data and associate
			ws = (WINDOW_STATE*) (((LPCREATESTRUCT)lParam)->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) ws);
			// Stackoverflow post said this was required to make the above change take effects
			SetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
			// Save window handle.. just in case I guess?
			ws->hwnd = hwnd;
			return DefWindowProc(hwnd, Message, wParam, lParam);
		} else {
			// Possible windows bug (mpv source says it is) that WM_GETMINMAXINFO may be passed before WM_NCCRREATE
			// So we return, and ignore it, since we don't have a WINDOW_STATE to handle it yet.
			return DefWindowProcW(hwnd, Message, wParam, lParam);
		}
	}
	// remove all that rubbish code
//make so it has to request in window_state_options which events it wants. (flags?) then only don't do default stuff with those, let them still call default and maybe it'll work


	//printf("Result:");
	//printf("ws: %p id: %#3X\n", ws, Message);
	if (ws == NULL) {
		printf("[WARN] WS was null when it shouldn't be\n");
		return 0;
	}

	//printf("ws: %p, id: %#3X\n", ws, Message);

	if (Message == WM_PAINT && (ws->event_flags & F_MWS_DRAW)) {
		//printf("WM_PAINT\n");
		mwslist_add(&(ws->events), create_full_mwsevent(MWS_DRAW, Message, wParam, lParam));
		//printf("\tmwslist, head: %p, next: %p\n", ws->events.head, ws->events.head == NULL ? NULL : ws->events.head);
		return 0;
	} else if (Message == WM_CLOSE) {
		if (ws->event_flags & F_MWS_CLOSE_WINDOW) {
			mwslist_add(&(ws->events), create_full_mwsevent(MWS_CLOSE_WINDOW, Message, wParam, lParam));
		} else {
			window_state_do_event_default(ws, create_full_mwsevent(MWS_CLOSE_WINDOW, Message, wParam, lParam));
		}
		return 0;
	} else if (Message == WM_DESTROY) {
		printf("[INFO] WndProc WM_DESTROY\n");
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, Message, wParam, lParam);

	/*switch(Message) {
		case WM_CLOSE:
			printf("[INFO] WndProc WM_CLOSE\n");
			DestroyWindow(hwnd);
		break;
		case WM_DESTROY:
			printf("[INFO] WndProc WM_DESTROY\n");
			PostQuitMessage(0);
		break;
		case WM_PAINT:
			printf("WM_PAINT");
			draw_all(ws);
		break;
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;*/
}

WINDOW_STATE* construct_window_state (WINDOW_STATE_OPTIONS opts) {
	WINDOW_STATE* ws = (WINDOW_STATE*) calloc(1, sizeof(WINDOW_STATE));
	if (ws == NULL) {
		free(ws);
		return NULL;
	}
	printf("Init dlist\n");
	dlist_init(&(ws->dlist));
	printf("Init mwslist\n");
	init_mwslist(&(ws->events));

	ws->event_flags = opts.event_flags;
	ws->should_end = false;

	ws->wc.cbSize = sizeof(WNDCLASSEX);
	// Redraw when it is resize
	ws->wc.style = CS_HREDRAW | CS_VREDRAW;
	ws->wc.lpfnWndProc = WndProc;
	ws->wc.cbClsExtra = 0;
	ws->wc.cbWndExtra = 0;
	ws->wc.hInstance = GetModuleHandle(NULL);
	ws->wc.hIcon = NULL;
	ws->wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	ws->wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	ws->wc.lpszMenuName = NULL;
	ws->wc.lpszClassName = g_szClassName;
	ws->wc.hIconSm = NULL;

	if(!RegisterClassEx(&(ws->wc))) {
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		free(ws);
		return 0;
	}

	ws->hwnd = CreateWindowEx(
		0, //WS_EX_CLIENTEDGE, // style
		g_szClassName, // class name
		opts.title, // window name
		WS_OVERLAPPEDWINDOW, // dwstyle
		opts.x,
		opts.y,
		opts.width,
		opts.height,
		NULL,
		NULL,
		ws->wc.hInstance,
		ws // pass this pointer-to-instance so it's tied to it
	);

	if(ws->hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		free(ws);
		return 0;
	}

	ws->defaultPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	ws->defaultBrush = CreateSolidBrush(RGB(0, 0, 0));

	ShowWindow(ws->hwnd, 1);
	UpdateWindow(ws->hwnd);

	return ws;
}

void free_window_state (WINDOW_STATE* wstate) {
	// TODO: do I need to free the dlist items
	DeleteObject(wstate->defaultPen);
	DeleteObject(wstate->defaultBrush);
	free(wstate);
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

bool should_window_state_end (WINDOW_STATE* wstate) {
	return wstate->should_end;
}
void window_state_update_tick (WINDOW_STATE* wstate) {
	if (GetMessage(&(wstate->Msg), NULL, 0, 0) > 0) {
		TranslateMessage(&(wstate->Msg));
		DispatchMessage(&(wstate->Msg));
	} else {
		wstate->should_end = true;
	}
}
void window_state_event_forced_handler (WINDOW_STATE* wstate, MWS_EVENT* evt) {
	// noop
}
MWS_EVENT get_window_state_event (WINDOW_STATE* wstate) {
	MWS_EVENT_LIST_node* node = mwslist_shift(&(wstate->events));
	if (node == NULL || node == NULL) {
		return (MWS_EVENT) {
			.type = MWS_NONE,
			.raw = NULL
		};
	} else {
		MWS_EVENT retevt = node->data;
		// We're obligated to free the node if we shift it off.
		free(node);
		window_state_event_forced_handler(wstate, &retevt);
		return retevt;
	}
}
void window_state_do_event_default (WINDOW_STATE* wstate, MWS_EVENT evt) {
	if (evt.type != MWS_NONE) {
		if (evt.raw->Message == WM_CLOSE) {
			DestroyWindow(wstate->hwnd);
			wstate->should_end = true;
			// WM_DESTROY is handled in wndproc
		}
		DefWindowProc(wstate->hwnd, evt.raw->Message, evt.raw->wParam, evt.raw->lParam);
	}
}

MWS_EVENT create_full_mwsevent (MWS_EVENT_TYPE type, UINT Message, WPARAM wParam, LPARAM lParam) {
	MWS_EVENT evt = create_mwsevent(type);
	// We have to allocate this ourselves if it doesn't exist, which it shouldn't
	// but I check just in case
	if (evt.raw == NULL) {
		evt.raw = (WINDOW_STATE_RAW_EVENT*) malloc(sizeof(WINDOW_STATE_RAW_EVENT));
	}
	evt.raw->Message = Message;
	evt.raw->wParam = wParam;
	evt.raw->lParam = lParam;
	return evt;
}