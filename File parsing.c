/*
 * ex1.c - David Levinov
 * ID   -  308152859
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 65536
#define DESTINATION_FILE_MODE S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH

extern int opterr, optind;

void exit_with_usage(const char *message) {
	fprintf (stderr, "%s\n", message);
	fprintf (stderr, "Usage:\n\tex1 [-f] SOURCE DEST BUFFER_SIZE\n");
	exit(EXIT_FAILURE);
}

void copy_file(const char *source_file, const char *dest_file, int buffer_size, int force_flag) {

	int sourceDescriptor, destinationDescriptor, bytesRead, bytesWrite;
	void* buffer[buffer_size];

	sourceDescriptor = open(source_file, O_RDONLY);
	// sourceDescriptor is -1 when opening the file was unsuccessful
	if(sourceDescriptor == -1)
	{
		perror("Unable to open source file");
		exit(EXIT_FAILURE);
	}

	// false force flag, will open the destination file, won't open if exists
	// else, force force flag true, will open and overwrite file if exists
	if(!force_flag)
	{
		destinationDescriptor = open(dest_file, O_WRONLY | O_CREAT | O_EXCL, DESTINATION_FILE_MODE);
	}
	else
	{
		destinationDescriptor = open(dest_file, O_WRONLY | O_CREAT | O_TRUNC, DESTINATION_FILE_MODE);
	}

	// -1 value indicates Unable to open destination file
	if(destinationDescriptor == -1)
	{
		perror("Unable to open destination file");
		exit(EXIT_FAILURE);
	}

	// Reading buffer_size bytes from file
	bytesRead = read(sourceDescriptor, &buffer, buffer_size);

	// If bytesRead < 0, failed reading from source file
	if(bytesRead < 0)
	{
		perror("Unable to read from source file");
		exit(EXIT_FAILURE);
	}

	// reading to buffer and copying from buffer to destination file.
	while(bytesRead > 0)
	{
		bytesWrite = write(destinationDescriptor, &buffer, buffer_size);

		// If bytesWrite < 0, couldn't write to destination file
		if(bytesWrite < 0)
		{
			perror("Unable to write to destination file");
			exit(EXIT_FAILURE);
		}

		// Reading buffer_size bytes from file
		bytesRead = read(sourceDescriptor, &buffer, buffer_size);

		// If bytesRead < 0, failed reading from source file
		if(bytesRead < 0)
		{
			perror("Unable to read from source file");
			exit(EXIT_FAILURE);
		}
	}

	printf("File %s was copied to %s\n", source_file, dest_file);
	close(sourceDescriptor);
	close(destinationDescriptor);
	exit(EXIT_SUCCESS);
}

void parse_arguments(
		int argc, char **argv,
		char **source_file, char **dest_file, int *buffer_size, int *force_flag) {
	/*
	 * parses command line arguments and set the arguments required for copy_file
	 */
	int option_character;

	opterr = 0; /* Prevent getopt() from printing an error message to stderr */

	while ((option_character = getopt(argc, argv, "f")) != -1) {
		switch (option_character) {
		case 'f':
			*force_flag = 1;
			break;
		default:  /* '?' */
			exit_with_usage("Unknown option specified");
		}
	}

	if (argc - optind != 3) {
		exit_with_usage("Invalid number of arguments");
	} else {
		*source_file = argv[argc-3];
		*dest_file = argv[argc-2];
		*buffer_size = atoi(argv[argc-1]);

		if (strlen(*source_file) == 0 || strlen(*dest_file) == 0) {
			exit_with_usage("Invalid source / destination file name");
		} else if (*buffer_size < 1 || *buffer_size > MAX_BUFFER_SIZE) {
			exit_with_usage("Invalid buffer size");
		}
	}
}

int main(int argc, char **argv) {
	int force_flag = 0; /* force flag default: false */
	char *source_file = NULL;
	char *dest_file = NULL;
	int buffer_size = MAX_BUFFER_SIZE;

	parse_arguments(argc, argv, &source_file, &dest_file, &buffer_size, &force_flag);

	copy_file(source_file, dest_file, buffer_size, force_flag);

	return EXIT_SUCCESS;
}


