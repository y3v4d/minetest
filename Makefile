.PHONY: cmake build run

cmake:
	cmake -B build

build:
	make -C build

run:
	./build/minetest