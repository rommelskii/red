#include <stdio.h>
#include <string.h>

#define FLAG_BUFFER_SIZE 2
// -c -u -r -d
// argv[] => <name> <flag (1)> <name/content (2)> <content (3)>




int parseFlag(const char *flag) {
/*
	parseFlag: returns corresponding flag type given flag string from argv

	argument/s: flag string from argv
*/
	
	if ( !strncmp(flag, "-c", 2) ) {
		return 0;
	}
	if ( !strncmp(flag, "-u", 2) ) {
		return 1;
	}
	if ( !strncmp(flag, "-r", 2) ) {
		return 2;
	}

	// fallback to 1 if no valid flag
	return 1;
}

int main(int argc, char *argv[]) {
	//housekeeping
	const char flagBuf[FLAG_BUFFER_SIZE];
	int arg_index = 1;
	if ( (argc <= arg_index) ) {
		fprintf(stderr, "Usage: red (-c -u -r) (file) (content)\n", argv[0]);
		fprintf(stderr, "Error: missing flag index", argc);
		return 1;
	}

	const char *FLAG = argv[1];

	//entry points
	const int flagType = parseFlag(FLAG);
	switch (flagType) {
		case 0: {
			//first check if there is an existing file
			FILE *pFile;
			const char *FILE_NAME = argv[2];
			pFile = fopen(FILE_NAME, "r");
			if ( pFile != NULL ) {
				printf("File '%s' exists!\n", FILE_NAME);
				return 1; 
			}
			fclose(pFile);
			
			pFile = fopen(FILE_NAME, "w");

			if (pFile == NULL) { //check if file creation fails
				perror("Failed to create file '%s'");
				fclose(pFile);
				return 1;
			}
			//check if there is content from argv
			if (argc == 4) {
				const char *CONTENT = argv[3];		
				if ( fputs(CONTENT, pFile) == EOF ) { // do fputs() and check if it fails
					perror("Error writing content");
					fclose(pFile);
					return 1;
				}
			}

			fclose(pFile);
			return 0;
			break;
		}
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
