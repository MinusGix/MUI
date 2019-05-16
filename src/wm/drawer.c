#include "drawer.h"

Drawer* allocate_drawer(Drawer drawer) {
	// TODO: allocate it, return it
	// use this function in the append/set functions that should use it
	Drawer* m_drawer = (Drawer*) malloc(sizeof(Drawer));

	memcpy(m_drawer, &drawer, sizeof(Drawer));

	return m_drawer;
}



CREATE_MLIST(DrawerList, dlist, Drawer)