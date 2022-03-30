.PHONY: cmake compile run

cmake:
	cmake -B build -G "MinGW Makefiles"

compile:
	mingw32-make -C build

run:
	./build/minetest.exe
