#ifndef FILE_M_WINDOWSTATE
#define FILE_M_WINDOWSTATE

#include "../mutil.h"
#include "drawer.h"

typedef struct WINDOW_STATE_DATA_t WINDOW_STATE;
typedef struct WINDOW_STATE_RAW_EVENT_t WINDOW_STATE_RAW_EVENT;

//. Note: any comments before an item with a +name means it works with that version
//. of window_state, and it also applies to any after it until you see a comment like -name
//. #name means it's possible but not implemented
//. ?name means it's unknown if it's possible, and not implemented
typedef struct WINDOW_STATE_OPTIONS_t {
	// +X11,+W32(D2D,GDI)(Somewhat iffy on w/h)
	// W32 Note: width/height when passed include the titlebar
	// or is not included? 100x100 w/h makes 161x134 which makes me think it's not
	// TODO: see if X11 does the same, and make sure they behave the same
	int width;
	int height;
	int x;
	int y;
	char* title;
	// +X11,?W32(D2D,GDI)
	int border_width;
	// Possibly have invert colors setting, X11 has black and white pixelt hing
	// Possible have background setting, X11WS sets it to white
	// Possibly have foreground setting, X11WS sets it to black
	unsigned long event_flags;
} WINDOW_STATE_OPTIONS;

typedef struct M_LRect_t {
	long x;
	long y;
	long width;
	long height;
} M_LRect;

typedef enum MWS_EVENT_TYPE_t {
	// There was no event
	MWS_NONE,
	// An event emitted which does not have an abstracted form
	// the raw property (see HWS_EVENT) will have the event, which can be looked at if wanted
	MWS_UK,
	// Draw to the screen
	MWS_DRAW,
	MWS_CLOSE_WINDOW,
	MWS_MOUSE_MOVE
} MWS_EVENT_TYPE;
typedef struct MWSE_MOUSE_MOVE_t {
	int x;
	int y;
} MWSE_MOUSE_MOVE;
typedef struct MWS_EVENT_t {
	MWS_EVENT_TYPE type;
	union {
		MWSE_MOUSE_MOVE emouse_move;
	};
	WINDOW_STATE_RAW_EVENT* raw;
} MWS_EVENT;

#define F_MWS_DRAW (1<<0) // 1
#define F_MWS_CLOSE_WINDOW (1<<1) // 10
#define F_MWS_MOUSE_MOVE (1<<2) // 100



WINDOW_STATE_OPTIONS window_state_options_defaults ();

WINDOW_STATE* construct_window_state_simple (int width, int height);
WINDOW_STATE* construct_window_state (WINDOW_STATE_OPTIONS opts);
void free_window_state (WINDOW_STATE* wstate);
void print_window_state (WINDOW_STATE* ws);

// TODO: add various can_get_window_state_width functions and the like
long get_window_state_width (WINDOW_STATE* ws);
long get_window_state_height (WINDOW_STATE* ws);
// Note: currently w32's xy pos is different than x11's, unsure as to why
long get_window_state_xpos (WINDOW_STATE* ws);
long get_window_state_ypos (WINDOW_STATE* ws);

DrawerList* get_window_state_dlist (WINDOW_STATE* wstate);
void add_drawer_to_window_state (WINDOW_STATE* wstate, Drawer drawer);

void pre_draw_all (WINDOW_STATE* wstate);
void draw_all (WINDOW_STATE* wstate);
void post_draw_all (WINDOW_STATE* wstate);
bool draw_drawer (WINDOW_STATE* wstate, Drawer* d);
bool draw_rectangle_drawer (WINDOW_STATE* wstate, RectangleDrawer* d);
bool draw_string_drawer (WINDOW_STATE* wstate, StringDrawer* d);

bool should_window_state_end (WINDOW_STATE* wstate);
bool window_state_updater (WINDOW_STATE* wstate);
void window_state_update_tick (WINDOW_STATE* wstate);
MWS_EVENT get_window_state_event (WINDOW_STATE* wstate);
// for when it should handle the events right before
// also allows cleaner code when handling events (as you're not also dispatching them)
// It should NOT keep the event around, as it's on the stack, if it needs to keep it then it should copy it
void window_state_event_forced_handler (WINDOW_STATE* wstate, MWS_EVENT* evt);
void window_state_do_event_default (WINDOW_STATE* wstate, MWS_EVENT evt);

MWS_EVENT create_mwsevent (MWS_EVENT_TYPE type);
void free_mwsevent (MWS_EVENT evt);

//CREATE_MLIST_HEADER(MWS_EVENT_LIST, mwslist, MWS_EVENT)
CREATE_MLINKEDLIST_HEADER(MWS_EVENT_LIST, mwslist, MWS_EVENT)

#endif