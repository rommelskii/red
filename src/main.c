#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BASE_10 10
#define EXIT_ERR 1
#define SUCCESS 0

int parseFlag(const char *flag);
int isNumber(char *str);

int main(int argc, char *argv[]) {
	//housekeeping
	const size_t FLAG_BUFFER_SIZE = 3;
	const size_t CONTENT_BUFFER_SIZE = 2048;
	const size_t MAX_UPDATE_SIZE = 2048;
	const int FLAG_INDEX = 1;
	if ( argc == 1 ) {
		fprintf(stderr, "Usage: red (-c -u -r) (file) (content)\n");
		fprintf(stderr, "Error: missing flag index");
		return EXIT_ERR;
	}
	if ( argc == 2 ) {
		fprintf(stderr, "Usage: red (-c -u -r) (file) (content)\n");
		fprintf(stderr, "Error: lacking arguments");
		return EXIT_ERR;
	}
	if ( argc > 5 ) {
		fprintf(stderr, "Usage: red (-c -u -r) (file) (content)\n");
		fprintf(stderr, "Error: too many arguments");
		return EXIT_ERR;
	}

	//extract flag
	const char flagBuf[FLAG_BUFFER_SIZE];
	strncpy(flagBuf, argv[FLAG_INDEX], FLAG_BUFFER_SIZE - 1);
	const int flagType = parseFlag(flagBuf);

	//entry points
	switch (flagType) {
		case 0: {
			if ( (argc != 3) && (argc != 4) ) {
				fprintf(stderr, "Error: incorrect parameters\n");
				return EXIT_ERR;
			}

			const size_t 	MAX_NAME_LENGTH 	= 2048;
			const size_t	MAX_WRITE_LENGTH 	= 2048 * 5;
			const size_t	MAX_LINE_NUMBER_LENGTH	= 9;

			const size_t 	FILENAME_INDEX 	= 2;
			const size_t	CONTENT_INDEX 	= 3;

			const char* 	ARG_FILE_NAME 	= argv[FILENAME_INDEX];
			const char*	ARG_CONTENT;

			const size_t 	LEN_ARG_FILENAME = strlen(ARG_FILE_NAME);
			size_t 		LEN_ARG_CONTENT;

			char* 		filename_buf;	
			char*		write_buf;

			FILE*		write_file;


			if (  LEN_ARG_FILENAME > MAX_NAME_LENGTH ) {
				fprintf(stderr, "Error: filename limit reached");
				return EXIT_ERR;
			}
			
			filename_buf = (char*)malloc( LEN_ARG_FILENAME * sizeof(char) );
			strncpy(filename_buf, ARG_FILE_NAME, LEN_ARG_FILENAME);

			//check first if file already exists
			write_file = fopen(filename_buf, "r");
			if (write_file != NULL) {
				fprintf(stderr, "Error: file already exists\n");
				return EXIT_ERR;
			} 
			fclose(write_file);
			
			//proceed creation
			write_file = fopen(filename_buf, "w");
			free(filename_buf);

			if (argc == 4)  {
				ARG_CONTENT = argv[CONTENT_INDEX];
				LEN_ARG_CONTENT = strlen(ARG_CONTENT);

				if (LEN_ARG_CONTENT > MAX_WRITE_LENGTH) {
					fprintf(stderr, "Error: write buffer limit reached\n");
					return EXIT_ERR;
				}

				write_buf = (char*)malloc( LEN_ARG_CONTENT * sizeof(char) );
				strncpy(write_buf, ARG_CONTENT, LEN_ARG_CONTENT);
				if ( fputs(write_buf, write_file) < 0 ) {
					fprintf(stderr, "Error: cannot write to file\n");	
					return EXIT_ERR;
				}
			}

			fclose(write_file);
			free(write_buf);
			return SUCCESS;	
		}
		case 1: {
			if ( (argc != 5) && (argc != 6) ) {
				fprintf(stderr, "Error: lacking update line or content\n");
				return EXIT_ERR;
			}

			FILE 		*writeFile;	
			FILE 		*readFile;

			const size_t 	UPDATE_STRING_INDEX = 4;
			const size_t 	LINE_NUMBER_INDEX = 3;
			const size_t 	FILE_NAME_INDEX = 2; 
			const size_t 	MAX_BUFFER_LENGTH = 2048;
			const size_t 	MAX_LINE_NUMBER_LENGTH = 9;

			const char* 	UPDATE_STRING = argv[UPDATE_STRING_INDEX];
			const char* 	LINE_NUMBER_STRING = argv[LINE_NUMBER_INDEX];
			const char* 	FILE_NAME = argv[FILE_NAME_INDEX];
			const size_t 	UPDATE_STRING_LENGTH = strlen(UPDATE_STRING);
			const size_t 	LINE_NUMBER_LENGTH = strlen(LINE_NUMBER_STRING);
			
			char 		*write_buffer;
			char 		*update_buffer; 
			char 		*line_buffer; 
			char 		*temporary_filename = "temp.txt";
			char 		*endptr;
			int 		 success_flag = 0; 
			unsigned long    line_number;
			unsigned long 	 current_line = 0;



			//buffer length checks
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

			if ( isNumber(line_buffer) < 0 ) {
				fprintf(stderr, "Error: line number provided is not a number\n");	
				return EXIT_ERR;
			}

			line_number = strtoul(line_buffer, &endptr, 10); //transform line buffer to line number

			printf("(Update buffer: %s, Line buffer: %s, Line number: %lu)\n", update_buffer, line_buffer, line_number);

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
				fgets(write_buffer, sizeof(write_buffer), readFile)
			      )
			{ 
				if (current_line == line_number) {
					printf("Line number reached: %lu\n", current_line);
					fprintf(writeFile, "%s\n", update_buffer);
					success_flag = 1;
				} else {
					fputs(write_buffer, writeFile);
				}
				current_line++;
			}

			// success flag is used to check whether the line extraction loop already added the update buffer
			if ( ( feof(readFile) ) && ( success_flag == 0 )  ) {
				if (line_number == 0) { //empty file case
					fputs(update_buffer, writeFile);
					printf("line number 0\n");
				} else { //add padding with newlines until update line is reached
					while(current_line <= line_number) { 
						fputs("\n", writeFile);
						current_line++;
					}
					fprintf(writeFile, "%s", update_buffer);
					printf("padding added\n");
				}
			}

			remove(FILE_NAME);
			rename(temporary_filename, FILE_NAME);

			fclose(readFile);
			fclose(writeFile);
			free(update_buffer);
			free(write_buffer);
			free(line_buffer);

			break;
		}
		case 2:
			// red -r hello.txt 0
			// argc always 4
			if (argc != 4) {
				fprintf(stderr, "Error: line needed\n");
				return EXIT_ERR;
			} else if ( argc > 4 ) {
				fprintf(stderr, "Error: too many arguments for remove\n");
				return EXIT_ERR;
			}

			//case 1: empty file
			//case 2: exceeding lines
			//case 3: normal

			/*
			 * program logic:
			 * 	- guards
			 *	- create read and temp
			 *	- copy 
			 *	- if eof, then case 1 or 2
			 *	- if success, do not write the current line and proceed as 
			 *        usual
			 *      - end
			 */



			break;
		case -1:
			printf("invalid flag!");
			break;
	}
	return SUCCESS;	
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

int isNumber(char *str) {
	const size_t STRING_LENGTH = strlen(str); 
	if (STRING_LENGTH == 0) {
		return -1;
	}
	char *iter = str;
	char *endptr = str+STRING_LENGTH;

	while ( iter != endptr ) {
		if ( isdigit(*iter) == 0 ) {
			return -1;
		}
		iter++;
	}

	return 1;
}
