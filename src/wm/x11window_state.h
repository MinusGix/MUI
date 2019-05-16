#ifndef FILE_M_X11WINDOWSTATE
#define FILE_M_X11WINDOWSTATE

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
//#include <X11/Xcms.h>
//#include <X11/Xutil.h>
//#include <X11/Xresource.h>
//#include <X11/keysym.h>

#include "window_state.h"
#include "../mutil.h"

#define KEY_ESCAPE 0x09
// TODO: it would be nice to be able to diffrentiate between KP_1 and KP_END
// see XLookupKeysym
#define KEY_KP_END 0x57
#define MWS_FLAGS unsigned long
typedef struct WINDOW_STATE_DATA_t {
	Display *dis;
	Window win;

	bool got_first_event;
	XEvent cur_event;
	
	int screen;

	// t_ prefix means tracked, aka this is updated by our code
	int t_width;
	int t_height;
	int t_x;
	int t_y;

	// c_ prefix means constant, aka this shouldn't be updated after we initialize it
	unsigned long c_black_pixel;
	unsigned long c_white_pixel;

	GC gc;

	DrawerList dlist;

	MWS_FLAGS event_flags;
	MWS_EVENT_LIST events;

	bool should_end;

	// Note: we store wm_delete_window the windowstate as I don't know if it's window-specific.
	// Default is `None`
	Atom wm_delete_window;
} WINDOW_STATE;

typedef struct WINDOW_STATE_RAW_EVENT_t {
	XEvent xevt;
} WINDOW_STATE_RAW_EVENT;

void update_window_state_dimensions (WINDOW_STATE* wstate, int width, int height);
void update_window_state_position (WINDOW_STATE* wstate, int x, int y);
void update_window_state_with_next_event (WINDOW_STATE* wstate);

M_LRect mlrect_from_x11XRectangle (XRectangle rect);
MWS_EVENT create_full_mwsevent (MWS_EVENT_TYPE type, XEvent xevt);

#endif