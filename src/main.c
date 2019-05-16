#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wm/wm.h"

/*Notes:
	LPSTR -> char* (Long Pointer String)
	UINT -> unsigned int
	WINAPI -> __stdcall
	LP prefix means Long Pointer
	LPC prefix means const long pointer

*/

void print_dimensions (WINDOW_STATE* wstate) {
	printf("w/h x/y: %ldx%ld (%ld, %ld)\n",
		get_window_state_width(wstate),
		get_window_state_height(wstate),
		get_window_state_xpos(wstate),
		get_window_state_ypos(wstate)
	);
}

#ifdef M_WINDOW_STATE_W32_D2D
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char *lpCmdLine, int nCmdShow) {
#endif
#ifdef M_WINDOW_STATE_W32_GDI
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char *lpCmdLine, int nCmdShow) {
#endif
#ifdef M_WINDOW_STATE_X11
	int main() {
#endif
	printf("Initialized\n");
	char *msg = "Hello, World!";
	WINDOW_STATE *wstate = construct_window_state((WINDOW_STATE_OPTIONS){
		.width = 100,
		.height = 100,
		.x = 10,
		.y = 10,
		.border_width = 1,
		.event_flags = F_MWS_DRAW | F_MWS_CLOSE_WINDOW | F_MWS_MOUSE_MOVE,
		.title = "Reality"
	});

	if (wstate == NULL) {
		printf("ERR IN OPENING DISPLAY\n");
		return 1;
	}

	print_window_state(wstate);

	add_drawer_to_window_state(wstate, (Drawer){
		.type = DRectangle,
		.drectangle = (RectangleDrawer){
			.x = 10,
			.y = 10,
			.width = 20,
			.height = 20,
			.should_fill = 1
		},
		.should_draw = 1
	});
	add_drawer_to_window_state(wstate, (Drawer){
		.type = DString,
		.dstring = (StringDrawer){
			.x = 10,
			.y = 50,
			.str = msg,
			.length = strlen(msg)
		},
		.should_draw = 1
	});

	while (window_state_updater(wstate)) {
		MWS_EVENT evt = get_window_state_event(wstate);

		if (evt.type == MWS_DRAW) {
			printf("MWS_DRAW\n");
			draw_all(wstate);
		} else if (evt.type == MWS_CLOSE_WINDOW) {
			printf("MWS_CLOSE_WINDOW\n");
			window_state_do_event_default(wstate, evt);
		} else if (evt.type == MWS_MOUSE_MOVE) {
			printf("MWS_MOUSE_MOVE (%d, %d)\n", evt.emouse_move.x, evt.emouse_move.y);
			window_state_do_event_default(wstate, evt);
		} else if (evt.type != MWS_NONE) {
			printf("Defaulted event: %d\n", (int)evt.type);
			window_state_do_event_default(wstate, evt);
		} else {
			// empty event
		}
	}
	printf("End of program\n");
	return 0;

#ifdef M_WINDOW_STATE_W32_D2D
	printf("IMPLEMENT EVENT LOOP FOR D2D\n");
	return 0;
#endif

#ifdef M_WINDOW_STATE_X11

	while (1) {
		update_window_state_with_next_event(wstate);
		print_dimensions(wstate);
		if (wstate->cur_event.type == Expose) {
			draw_all(wstate);
			// display, Drawable, GC x, y, width, height
			//XFillRectangle(wstate->dis, wstate->win, wstate->gc, 20, 20, 10, 10);
			// display, Drawable, GC, x, y, string, string length
			//XDrawString(wstate->dis, wstate->win, wstate->gc, 10, 50, msg, strlen(msg));
		} else if (wstate->cur_event.type == KeyPress) {
			unsigned int keycode = wstate->cur_event.xkey.keycode;
			if (keycode == KEY_ESCAPE) {
				break;
			}
		} else if (wstate->cur_event.type == KeyRelease) {
			unsigned int keycode = wstate->cur_event.xkey.keycode;
			if (keycode == KEY_KP_END) {
				print_window_state(wstate);
			}
		} else if (wstate->cur_event.type == ConfigureNotify) {
			update_window_state_dimensions(wstate, wstate->cur_event.xconfigure.width, wstate->cur_event.xconfigure.height);
			update_window_state_position(wstate, wstate->cur_event.xconfigure.x, wstate->cur_event.xconfigure.y);
		}
	}

	XCloseDisplay(wstate->dis);
#endif
		return 0;
	}