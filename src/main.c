#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BASE_10 10

int parseFlag(const char *flag);

int main(int argc, char *argv[]) {
	//housekeeping
	const size_t FLAG_BUFFER_SIZE = 3;
	const size_t CONTENT_BUFFER_SIZE = 2048;
	const size_t MAX_UPDATE_SIZE = 2048;
	const int FLAG_INDEX = 1;
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
			// red -u hello.txt 0 wow
			// 0    1      2    3   4
			FILE *writeFile;	
			FILE *readFile;

			const size_t UPDATE_STRING_INDEX = 4;
			const size_t LINE_NUMBER_INDEX = 3;
			const size_t FILE_NAME_INDEX = 2; 
			const size_t MAX_BUFFER_LENGTH = 2048;
			const size_t MAX_LINE_NUMBER_LENGTH = 9;

			const char* UPDATE_STRING = argv[UPDATE_STRING_INDEX];
			const char* LINE_NUMBER_STRING = argv[LINE_NUMBER_INDEX];
			const char* FILE_NAME = argv[FILE_NAME_INDEX];
			const size_t UPDATE_STRING_LENGTH = strlen(UPDATE_STRING);
			const size_t LINE_NUMBER_LENGTH = strlen(LINE_NUMBER_STRING);
			
			char *write_buffer;
			char *update_buffer; 
			char *line_buffer; 
			char *temporary_filename = "temp.txt";
			char *endptr;
			unsigned long line_number;
			unsigned long current_line = 0;



			// guards against buffer lengths
			if ( UPDATE_STRING_LENGTH > MAX_BUFFER_LENGTH ) {
				fprintf(stderr, "Error: max content buffer length reached");
				return 1;
			}
			if ( LINE_NUMBER_LENGTH > MAX_LINE_NUMBER_LENGTH ) {
				fprintf(stderr, "Error: max line buffer length reached");
				return 1;
			}

			//allocate sizes to buffers
			write_buffer = (char *)malloc(MAX_BUFFER_LENGTH * sizeof(char));
			update_buffer = (char *)malloc(UPDATE_STRING_LENGTH * sizeof(char));
			line_buffer = (char *)malloc(LINE_NUMBER_LENGTH * sizeof(char));


			//copy arguments to respective buffers
			strncpy(update_buffer, UPDATE_STRING, strlen(argv[UPDATE_STRING_INDEX]));
			strncpy(line_buffer, LINE_NUMBER_STRING, strlen(argv[LINE_NUMBER_INDEX]));

			line_number = strtoul(line_buffer, &endptr, 10); //transform line buffer to line number
									 //
			printf("DEBUG: update_buffer, line_buffer, line_number = ");
			printf("(%s,%s,%lu)\n", update_buffer, line_buffer, line_number);

			//file I/O
			readFile = fopen(FILE_NAME, "r");
			writeFile = fopen(temporary_filename, "w");

			if ( readFile == NULL ) {
				fprintf(stderr, "Error: %s not found", FILE_NAME);
				return 1;
			}
			if ( writeFile == NULL ) {
				fprintf(stderr, "Error: error creating temp file");
				return 1;
			}

			//begin line extraction
			while (
				fgets(write_buffer, sizeof(write_buffer) - 1, readFile)
			      )
			{ 
				if (current_line == line_number) {
					fprintf(writeFile, "%s\n", write_buffer);
					printf("Line number reached: %lu\n", current_line);
				} else {
					fputs(write_buffer, writeFile);
				}
				current_line++;
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

