main: src/main.c
	gcc src/main.c -o bin/main

test:
	./bin/main

clean:
	rm -f bin/main
