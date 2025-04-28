#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parseFlag(const char *flag);

int main(int argc, char *argv[]) {
	//housekeeping
	const size_t FLAG_BUFFER_SIZE = 3;
	const size_t CONTENT_BUFFER_SIZE = 2048;
	const size_t MAX_UPDATE_SIZE = 2048;
	const int FLAG_INDEX;
	if ( argc == 1 ) {
		fprintf(stderr, "Usage: red (-c -u -r) (file) (content)\n");
		fprintf(stderr, "Error: missing flag index");
		return 1;
	}
	if ( argc == 2 ) {
		fprintf(stderr, "Usage: red (-c -u -r) (file) (content)\n");
		fprintf(stderr, "Error: lacking arguments");
		return 1;
	}
	if ( argc > 5 ) {
		fprintf(stderr, "Usage: red (-c -u -r) (file) (content)\n");
		fprintf(stderr, "Error: too many arguments");
		return 1;
	}

	//extract flag
	const char flagBuf[FLAG_BUFFER_SIZE];
	strncpy(flagBuf, argv[FLAG_INDEX], FLAG_BUFFER_SIZE - 1);
	const int flagType = parseFlag(flagBuf);

	//entry points
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
		case 1: {
			if (argc < 5) {
				fprintf(stderr, "Error: specify file, line number, and content");
				return 1;
			}
			// red -u mems.txt 0 a
			const char *FILE_NAME = argv[2];
			const long int LINE_NUMBER = strtol(argv[3], NULL, 0);
			const char *content_argument = argv[4];
			const char content_buffer[CONTENT_BUFFER_SIZE];
			const char line_update_buffer[MAX_UPDATE_SIZE];
			int current_line = 0;
			int success = 1;

			if ( sizeof(content_argument) > MAX_UPDATE_SIZE ) { //check if buffer size is exceeded
				fprintf(stderr, "Error: maximum update size reached");
				return 1;
			}

			strcpy(line_update_buffer, content_argument); //extract content argument to buffer
								      //
			fprintf(stdout, "%s\n", line_update_buffer);

			FILE *inputFile;
			FILE *tempFile;

			inputFile = fopen(FILE_NAME, "r"); //check if file to be updated exists
			if (inputFile == NULL) {
				fprintf(stderr, "Error: file '%s' does not exist for updating", FILE_NAME);
				return 1;
			}

			if ( fgetc(inputFile) == EOF ) {
				fprintf(stderr, "Error: cannot edit an empty file");
				return 1;
			}

			tempFile = fopen("temp.txt", "w");

			if ( tempFile == NULL ) {
				perror("Error: error in writing to file");
				return 1;
			}

			//begin extracting each line from original file
			//populate until you reach the line to be update
			//falls back to error if EOF is reached
			while ( fgets(content_buffer, sizeof(content_buffer), inputFile) != NULL ) {
				if (current_line == LINE_NUMBER) {
					if ( fputs(line_update_buffer, tempFile) < 0 ) {
						fprintf(stderr, "Error: error writing content to file");
						success = 0;
					}
					break;
				} else {
					fputs(content_buffer, tempFile);
				}
			}


			if ( feof(inputFile) ) {
				if (current_line == LINE_NUMBER) {
					fputs(line_update_buffer, tempFile);
				} else if ( current_line < LINE_NUMBER ) {
					fprintf(stderr, "Error: line exceeds existing file buffer");
					success = 0;
				}
			}


			if (success) {
				fprintf(stdout, "File '%s' updated sucessfully at line: %li", FILE_NAME, LINE_NUMBER);	
				fclose(tempFile);
				fclose(inputFile);
				return 0;
			} else {
				return 1;
			}

			break;
		}
		case 2:
			printf("-r detected");
			break;
		case -1:
			printf("invalid flag!");
			break;
	}
	return 0;	
}

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
	return -1;
}
