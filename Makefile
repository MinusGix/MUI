all_files = ./src/main.c ./src/wm/window_state.c ./src/wm/drawer.c ./src/mutil.c
linux_files = $(all_files)
linux_x11_files = $(linux_files) ./src/wm/x11window_state.c
windows_files = $(all_files)
windows_direct2d_files = $(windows_files) ./src/wm/d2d_w32window_state.c
windows_gdi_files = $(windows_files) ./src/wm/gdi_w32window_state.c

build_output = build/program

linux_compiler = gcc
windows_compiler = i686-w64-mingw32-gcc

start-linux-x11 : build-linux-x11
	./$(build_output)

build-linux-x11 : clean
	mkdir ./build
	$(linux_compiler) -g -DM_WINDOW_STATE_X11 -Wall -Wextra -Winit-self -Wuninitialized -pedantic -g -o ./$(build_output) $(linux_x11_files) -L/usr/X11/lib -lX11

start-windows-direct2d: build-windows-direct2d
	wine ./$(build_output)

build-windows-direct2d : clean
	mkdir ./build
	i686-w64-mingw32-gcc -g -mwindows -DM_WINDOW_STATE_W32_D2D -o ./$(build_output).exe $(windows_direct2d_files) -luser32 -lkernel32 -lusp10 -lgdi32 -lcomctl32 -luxtheme -lmsimg32 -lcomdlg32 -ld2d1 -ldwrite -lole32 -loleaut32 -loleacc -luuid
	cp -r src/resources build/resources

start-windows-gdi: build-windows-gdi
	wine ./$(build_output).exe

build-windows-gdi : clean
	mkdir ./build
	$(windows_compiler) -mwindows -DM_WINDOW_STATE_W32_GDI -o ./$(build_output).exe $(windows_gdi_files) -luser32 -lkernel32 -lusp10 -lgdi32 -lcomctl32 -luxtheme -lmsimg32 -lcomdlg32 -lole32 -loleaut32 -loleacc -luuid
	cp -r ./src/resources ./build/resources

clean :
	rm -rdf ./build