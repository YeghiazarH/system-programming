#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>



int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("File name was not given by arguments\n");
		exit(1);
	}

	int file = open(argv[1], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);
	if (file == -1)
	{
		printf("Something went wrong while opening the file. Error %s\n", strerror(errno));
		exit(errno);
	}

	int fileDup = dup(file);
	if (fileDup == -1)
	{
		printf("Something went wrong while opening the file. Error %s\n", strerror(errno));
		close(file);
		exit(errno);
	}

	const char* str1 = "fisrt line\n";
	const char* str2 = "secod line\n";

	if (write(file, str1, strlen(str1)) < 0)
	{
		printf("Something went wrong while opening the file. Error %s\n", strerror(errno));
		close(file);
		close(fileDup);
		exit(errno);
	}

	if (write(fileDup, str2, strlen(str2)) < 0)
	{
		printf("Something went wrong while opening the file. Error %s\n", strerror(errno));
		close(file);
		close(fileDup);
		exit(errno);
	}

	close(file);
	close(fileDup);

	return 0;
}
