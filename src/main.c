#include <stdio.h>
#include <string.h>


// -c -u -r -d
// argv[] => <name> <flag (1)> <content (2)>


int parseFlag(const char *flag) {
/*
	parseFlag: returns corresponding flag type given flag string from argv

	argument/s: flag string from argv
*/
	
	if ( !strcmp(flag, "-c") ) {
		return 0;
	}
	if ( !strcmp(flag, "-u") ) {
		return 1;
	}
	if ( !strcmp(flag, "-r") ) {
		return 2;
	}

	// fallback to -1 if no valid flag
	return -1;
}

int main(int argc, char *argv[]) {
	const char *FLAG = argv[1];
	if ( (argc < 3 || argc > 4) ) {
		printf("Usage: red (-c -u -r) (file) (content)");
		printf("\n");
		return -1;
	}

	//entry points
	const int flagType = parseFlag(FLAG);
	switch (flagType) {
		case 0:
			printf("-c detected");
			break;
		case 1:
			printf("-u detected");
			break;
		case 2:
			printf("-r detected");
			break;
		default:
			printf("invalid flag!");
			break;
	}
	return 0;	
}
