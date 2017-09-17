main:
	gcc screenshot.c -lX11 -o run

run: main
	./run

clean:
	rm -f *.o *.gch run

install: main
	cp run /usr/local/bin/ambient_light
