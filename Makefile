build:
	@meson setup --reconfigure build
compile: build
	@meson compile -C build
run: compile
	@./build/main
test: build
	@meson test -C build
