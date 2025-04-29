#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
			//check if arguments are good
			//check if file exists
			//create dummy file
			//loop until current line = target_line or EOF is reached
			//if target_line reached, replace with buffer; otherwise continue looping until target_line
			// is reached
			
			if ( argc < 5 ) {
				fprintf(stderr, "Error: incomplete fields");
				return 1;
			}

			FILE *referenceFile;
			FILE *temporaryFile;
			const size_t WRITE_MAX_SIZE = 2048 * 5;
			const size_t UPDATE_MAX_SIZE = 2048 * 5;
			const size_t FILE_NAME_INDEX = 2;
			const size_t LINE_NUMBER_INDEX = 3;
			const size_t CONTENT_INDEX = 4;
			size_t total_size = 0;
			const char* FILE_NAME = argv[FILE_NAME_INDEX];
			char write_buf[WRITE_MAX_SIZE];
			char update_buf[WRITE_MAX_SIZE];
			char *temporary_filename[2048];
			int current_line = 0;
			long int target_line = 0;
			int is_success = 1;

			referenceFile = fopen(FILE_NAME, "r");	 // check if file exists
			if (referenceFile == NULL) {
				fprintf(stderr, "Error: '%s' does not exist!", FILE_NAME);
				return 1;
			}

			strncpy(temporary_filename, "temp.txt", sizeof(temporary_filename));

			temporaryFile = fopen("temp.txt", "w");
			if (temporaryFile == NULL) {
				fprintf(stderr, "Error: cannot write temporary file");
				return 1;
			}

			target_line = atol( argv[LINE_NUMBER_INDEX] );
			strncpy(update_buf, argv[CONTENT_INDEX], sizeof(update_buf));

			while ( fgets(write_buf, sizeof(write_buf), referenceFile) != NULL ) {
				if (current_line == target_line) {
					if ( fputs(update_buf, temporaryFile) < 0 ) {
						fprintf(stderr, "Error: error writing to '%s'", FILE_NAME);
						is_success = 0;
						break;
					}
					total_size = total_size + strlen(update_buf);
					break;
				} else {
					if ( fputs(write_buf, temporaryFile) < 0 ) {
						fprintf(stderr, "Error: error writing to '%s'", FILE_NAME);
						is_success = 0;
						break;
					}
					total_size = total_size + strlen(write_buf);
				}
				current_line++;
			}

			if ( feof(referenceFile) ) {
				//continue incrementing and add padding until before target line is reached
				while ( current_line < target_line ) {
					fprintf(temporaryFile, "\n");
					current_line++;
				}
				fprintf(temporaryFile, "%s", update_buf);
				total_size = total_size + strlen(update_buf);
			} else if (is_success == 0) {
				fprintf(stderr, "Error: cannot write to file");
			}

			if (is_success == 1) {
				if ( (remove(FILE_NAME) != 0) ||  (rename(temporary_filename, FILE_NAME) != 0) ) {
					fprintf(stderr, "Error: failed to overwrite '%s'", FILE_NAME);
					return 1; 	
				}
				fclose(referenceFile);
				fclose(temporaryFile);
				fprintf(stdout, "success: wrote %zu bytes!\n", total_size);
				return 0;
			} else {
				fclose(referenceFile);
				fclose(temporaryFile);
				fprintf(stdout, "Error has occured\n");
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
