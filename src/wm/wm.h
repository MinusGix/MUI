// I would like to make so if neither _W32 and _X11 it will default to _X11

//#ifndef M_WINDOW_STATE_X11
//	#ifndef
//#endif

// TODO: make these exclusive, so only one will be required ( needs something like elseifdef )
#ifdef M_WINDOW_STATE_X11
	#include "x11window_state.h"
#endif
#ifdef M_WINDOW_STATE_W32_D2D
	#include "d2d_w32window_state.h"
#endif
#ifdef M_WINDOW_STATE_W32_GDI
	#include "gdi_w32window_state.h"
#endif