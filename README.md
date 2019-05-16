# A C GUI library
I'll go with the name 'MUI'.
The goal of this is to abstract different GUI libraries for operating systems so you can write them in the same way, while exposing the original apis if you so wish to use them. (You'll have to use ifdefs and the like, but it's a goal)
Current state:
	- See /notes/compat.ods for a spreadsheet on how much is supported.
	- Abstracts Linux-x11, Windows-GDI, Windows-D2D (Direct2D)
		- Windows-D2D is currently far behind the others and in a non-usable state.
		- x11,GDI both currently only support abstracting away creation of window (not fully detailed yet), drawing of simple objects (black rectangle, very incomplete text, adding more should be easy). and two events Drawing events  and close events

