#ifndef FILE_M_DRAWER
#define FILE_M_DRAWER

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../mutil.h"


// Note: This should be stand-alone as much as possible to increase portability.

#define DRAWER_LIST_INITIAL_CAPACITY 30

// I wish this wasn't in global scope..
typedef enum DrawerType_t {
	// A drawable/fillable rect
	DRectangle,
	// String
	DString,
	// No type, perhaps it was killed
	DNONE,
} DrawerType;

typedef struct RectangleDrawer_t {
	// Basically copied from XRectangle
	short x;
	short y;
	unsigned short width;
	unsigned short height;

	unsigned short should_fill : 1;
} RectangleDrawer;
typedef struct StringDrawer_t {
	// TODO: add font_size options
	// TODO: add bold/italic/underlined etc options
	// TODO: add font options
	int x;
	int y;
	int length;
	char* str;
} StringDrawer;

typedef struct Drawer_t {
	DrawerType type;
	union {
		RectangleDrawer drectangle;
		StringDrawer dstring;
	};
	// == Flags
	unsigned short should_draw : 1;
} Drawer;

CREATE_MLIST_HEADER(DrawerList, dlist, Drawer)

Drawer* allocate_drawer (Drawer drawer);


#endif