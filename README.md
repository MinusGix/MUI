# A C GUI library
I'll go with the name 'MWS' as that's a common prefix in this.
The goal of this is to abstract different GUI libraries for operating systems so you can write them in the same way, while exposing the original apis if you so wish to use them. (You'll have to use ifdefs and the like, but it's a goal)
Current state:
	- Abstracts Linux-x11, Windows-GDI, Windows-D2D (Direct2D)
		- Windows-D2D is currently far behind the others and in a non-usable state.
		- x11,GDI both currently only support abstracting away creation of window (not fully detailed yet), drawing of simple objects (Rectangle, very incomplete text, adding more should be easy). and two events Drawing events (Expose, WM_PAINT, respectively), and hopefully close events (?? I don't know ???, WM_CLOSE)
		- Note that according to a stackoverflow answer I found, there is no window close event for x11
