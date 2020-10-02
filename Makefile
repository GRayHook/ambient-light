.PHONY: all prepare run clean install

all: prepare build/ambient_light

build/ambient_light: src/ambient_light.c src/ambient_light.h
	gcc src/ambient_light.c -lX11 -o build/ambient_light

./build:
	mkdir build -p

prepare: ./build

run: all
	./build/ambient_light

clean:
	rm -rf build

install: all
	cp build/ambient_light /usr/local/bin/ambient_light
