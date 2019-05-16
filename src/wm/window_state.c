#include "window_state.h"
#include <assert.h>
#include <stdio.h>

WINDOW_STATE_OPTIONS window_state_options_defaults () {
	return (WINDOW_STATE_OPTIONS) {
		.width = 100,
		.height = 100,
		.x = 10,
		.y = 10,
		.border_width = 1,
		.event_flags = F_MWS_DRAW,
		.title = "",
	};
}

void draw_all (WINDOW_STATE* wstate) {
	pre_draw_all(wstate);

	DrawerList* list = get_window_state_dlist(wstate);
	int len = dlist_length(list);
	for (int i = 0; i < len; i++) {
		Drawer* item = dlist_get(list, i);
		if (item != NULL) {
			draw_drawer(wstate, item);
		}
	}

	post_draw_all(wstate);
}

// TODO: add version of draw_drawer which checks if it is null or not
bool draw_drawer (WINDOW_STATE* wstate, Drawer* d) {
	switch (d->type) {
		case DRectangle:
			return draw_rectangle_drawer(wstate, &d->drectangle);
			break;
		case DString:
			return draw_string_drawer(wstate, &d->dstring);
			break;
		default:
			return false;
	}
}

MWS_EVENT create_mwsevent (MWS_EVENT_TYPE type) {
	return (MWS_EVENT) {
		.type = type,
		.raw = NULL
	};
}
// THe only part we need to free is the raw part
void free_mwsevent (MWS_EVENT evt) {
	if (evt.raw != NULL) {
		// TODO: possibly a flag which decides if it should include raw events in the struct?
		// If you don't need them, then why include them?
		free(evt.raw);
	}
}

CREATE_MLINKEDLIST(MWS_EVENT_LIST, mwslist, MWS_EVENT)


bool window_state_updater (WINDOW_STATE* wstate) {
	// We check this before we update the tick, because
	// 1. Updating the tick on a purposely dead window is possibly bad
	// 2. x11 likes waiting for event if there is none, which means if we kill the window it will wait for nothing.
	if (should_window_state_end(wstate)) {
		return false;
	}
	window_state_update_tick(wstate);
	return !should_window_state_end(wstate);
}