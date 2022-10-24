all: clean build run
build:
	gcc main.c PPM.c quadtree.c -std=gnu11 -o quadtree
clean:
	rm quadtree
run:
	valgrind --track-origins=yes --leak-check=full -s ./quadtree -m v 370 test1.ppm out