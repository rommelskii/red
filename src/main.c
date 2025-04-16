#include <stdio.h>

// -c -u -r -d
// argv[] => <name> <flag (1)> <content (2)>

int main(int argc, char *argv[]) {
	if ( argc != 2 ) {
		printf("Usage: red (-c -u -r) (file) (content)");
		printf("\n");
	}
}
