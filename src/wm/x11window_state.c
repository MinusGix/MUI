#include "x11window_state.h"
#include <stdio.h>
#include <stdlib.h>

WINDOW_STATE* construct_window_state_simple (int width, int height) {
	WINDOW_STATE_OPTIONS opts = window_state_options_defaults();
	opts.width = width;
	opts.height = height;
	return construct_window_state(opts);
}

WINDOW_STATE* construct_window_state (WINDOW_STATE_OPTIONS opts) {
	WINDOW_STATE* wstate = (WINDOW_STATE*) malloc(sizeof(WINDOW_STATE));
	if (wstate == NULL) {
		return NULL;
	}

	wstate->wm_delete_window = None;

	dlist_init(&(wstate->dlist));
	init_mwslist(&(wstate->events));

	wstate->should_end = false;
	wstate->event_flags = opts.event_flags;

	wstate->t_height = opts.height;
	wstate->t_width = opts.width;
	wstate->t_x = opts.x;
	wstate->t_y = opts.y;

	wstate->dis = XOpenDisplay(NULL);
	if (wstate->dis == NULL) {
		free(wstate);
		return NULL;
	}
	wstate->screen = DefaultScreen(wstate->dis);

	wstate->c_black_pixel = BlackPixel(wstate->dis, wstate->screen);
	wstate->c_white_pixel = WhitePixel(wstate->dis, wstate->screen);

	wstate->gc = DefaultGC(wstate->dis, wstate->screen);
	XSetBackground(wstate->dis, wstate->gc, wstate->c_white_pixel);
    XSetForeground(wstate->dis, wstate->gc, wstate->c_black_pixel);

	wstate->win = XCreateSimpleWindow(
		wstate->dis, // display
		RootWindow(wstate->dis, wstate->screen), // parent
		opts.x, opts.y, // x, y
		opts.width, opts.height,
		opts.border_width, // border_width
		wstate->c_black_pixel, // border
		wstate->c_white_pixel // background
	);

	long flags = 0;

	if (wstate->event_flags & F_MWS_DRAW) {
		flags |= ExposureMask;
	}

	XSelectInput(
		wstate->dis,
		wstate->win,
		flags
	);
	XMapWindow(wstate->dis, wstate->win);

	if (wstate->event_flags & F_MWS_CLOSE_WINDOW) {
		// Seems to have to be done after selectinput/mapwindow
		// See: https://stackoverflow.com/questions/1157364/intercept-wm-delete-window-on-x11
		wstate->wm_delete_window = XInternAtom(wstate->dis, "WM_DELETE_WINDOW", False);
		XSetWMProtocols(wstate->dis, wstate->win, &wstate->wm_delete_window, 1);
	}

	// Makes so KeyRelease's only happen once
	XkbSetDetectableAutoRepeat(wstate->dis, true, NULL);

	return wstate;
}
void free_window_state (WINDOW_STATE* wstate) {
	// TODO: do I need to free the dlist items
	XFreeGC(wstate->dis, wstate->gc);
	XDestroyWindow(wstate->dis, wstate->win);
	XCloseDisplay(wstate->dis);

	free(wstate->dis);
	free(wstate);
}
// For debugging
void print_window_state (WINDOW_STATE* ws) {
	printf(
		"WINDOW_STATE {\n\t"
			"dis: %s,\n\t"
			"win: [UNPRINTABLE],\n\t"
			"cur_event(type): %d,\n\t"
			"screen: %d\n\t"
			"t_width: %d\n\t"
			"t_height: %d\n\t"
			"t_x: %d\n\t"
			"t_y: %d\n\t"
			"c_black_pixel: %lu\n\t"
			"c_white_pixel: %lu\n\t"
			"gc: [UNPRINTABLE],\n\t"
			"dlist: %ld size, %ld capacity\n"
		"}\n",
		ws->dis == NULL ? "NULL" : "NON-NULL",
		ws->cur_event.type,
		ws->screen,
		ws->t_width,
		ws->t_height,
		ws->t_x,
		ws->t_y,
		ws->c_black_pixel,
		ws->c_white_pixel,
		dlist_length(&(ws->dlist)),
		ws->dlist.capacity
	);
}

// This assumes you pass in correct data.
void update_window_state_dimensions (WINDOW_STATE* wstate, int width, int height) {
	wstate->t_height = height;
	wstate->t_width = width;
}
void update_window_state_position (WINDOW_STATE* wstate, int x, int y) {
	wstate->t_x = x;
	wstate->t_y = y;
}

void update_window_state_with_next_event (WINDOW_STATE* wstate) {
	XNextEvent(wstate->dis, &wstate->cur_event);
}

// TODO: perhaps store them as longs?
long get_window_state_width (WINDOW_STATE* wstate) {
	return (long) wstate->t_width;
}
long get_window_state_height (WINDOW_STATE* wstate) {
	return (long) wstate->t_height;
}
long get_window_state_xpos (WINDOW_STATE* wstate) {
	return (long) wstate->t_x;
}
long get_window_state_ypos (WINDOW_STATE* wstate) {
	return (long) wstate->t_y;
}

// ==== Manipulating drawing list

DrawerList* get_window_state_dlist (WINDOW_STATE* wstate) {
	return &(wstate->dlist);
}
void add_drawer_to_window_state (WINDOW_STATE* wstate, Drawer drawer) {
	dlist_append(get_window_state_dlist(wstate), allocate_drawer(drawer));
}


// ===== Drawing

bool draw_rectangle_drawer (WINDOW_STATE* wstate, RectangleDrawer* d) {
	if (d->should_fill) {
		XFillRectangle(wstate->dis, wstate->win, wstate->gc, d->x, d->y, d->width, d->height);
	} else {
		XDrawRectangle(wstate->dis, wstate->win, wstate->gc, d->x, d->y, d->width, d->height);
	}
	return true;
}

bool draw_string_drawer (WINDOW_STATE* wstate, StringDrawer* d) {
	XDrawString(wstate->dis, wstate->win, wstate->gc, d->x, d->y, d->str, d->length);
	return true;
}

void pre_draw_all (WINDOW_STATE* wstate) {}
void post_draw_all (WINDOW_STATE* wstate) {}

bool should_window_state_end (WINDOW_STATE* wstate) {
	return wstate->should_end;
}
void window_state_update_tick (WINDOW_STATE* wstate) {
	update_window_state_with_next_event(wstate);

	XEvent evt = wstate->cur_event;
	int etype = evt.type;
	MWS_FLAGS flags = wstate->event_flags;

	// TODO: check if Expose is for drawing, or is it GraphicsExpose?
	// if the above todo does anything, make sure to change the flags
	if (etype == Expose && (flags & F_MWS_DRAW)) {
		mwslist_add(&(wstate->events), create_full_mwsevent(MWS_DRAW, wstate->cur_event));
	} else if (etype == ClientMessage) {
		if ((Atom)evt.xclient.data.l[0] == wstate->wm_delete_window) {
			mwslist_add(&(wstate->events), create_full_mwsevent(MWS_CLOSE_WINDOW, wstate->cur_event));
		}
	}
	// TODO: add close event
}
MWS_EVENT create_full_mwsevent (MWS_EVENT_TYPE type, XEvent xevt) {
	MWS_EVENT evt = create_mwsevent(type);
	if (evt.raw == NULL) {
		evt.raw = (WINDOW_STATE_RAW_EVENT*) malloc(sizeof(WINDOW_STATE_RAW_EVENT));
	}
	// TODO: make sure this has nothing that will be overwritten when you update the event
	// to make sure that saving this won't cause issues
	evt.raw->xevt = xevt;

	return evt;
}

void window_state_do_event_default (WINDOW_STATE* wstate, MWS_EVENT evt) {
	if (evt.type == MWS_CLOSE_WINDOW) {
		XDestroyWindow(wstate->dis, wstate->win);
		wstate->should_end = true;
	}
}

void window_state_event_forced_handler (WINDOW_STATE* wstate, MWS_EVENT* evt) {}

MWS_EVENT get_window_state_event (WINDOW_STATE* wstate) {
	MWS_EVENT_LIST_node* node = mwslist_shift(&(wstate->events));
	if (wstate->should_end || node == NULL || node == NULL) {
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

M_LRect mlrect_from_x11XRectangle (XRectangle rect) {
	return (M_LRect) {
		.x = rect.x,
		.y = rect.y,
		.width = rect.width,
		.height = rect.height
	};
}