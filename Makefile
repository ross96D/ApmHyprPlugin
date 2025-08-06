VERSION = HEAD
PWD = $$(pwd)

.PHONY: clean build

main.so: main.cpp
	c++ -std=c++23 --no-gnu-unique -Wall -g  $$(pkg-config --cflags pixman-1 libdrm hyprland) -o main.so main.cpp --shared -fPIC

build: main.so

clean:
	rm -f main.so

load:
	hyprctl plugin load $(PWD)/main.so

unload:
	hyprctl plugin unload $(PWD)/main.so

list:
	hyprctl plugin list

version:
	echo $(VERSION)