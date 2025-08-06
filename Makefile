VERSION = HEAD
PWD = $$(pwd)

.PHONY: clean build

main.so: main.cpp
	c++ -std=c++23 --no-gnu-unique -Wall -g  $$(pkg-config --cflags pixman-1 libdrm hyprland) -o main.so main.cpp --shared -fPIC

hash: main.so
	shasum main.so | awk '{print $$1}' > hash


build: hash
	mkdir $(PWD)/build/$$(cat hash) 2>/dev/null; cp main.so $(PWD)/build/$$(cat hash)/main.so

clean:
	rm -f main.so

load:
	hyprctl plugin load $(PWD)/build/$$(cat hash)/main.so

unload:
	hyprctl plugin unload $(PWD)/build/$$(cat hash)/main.so

list:
	hyprctl plugin list

version:
	echo $(VERSION)