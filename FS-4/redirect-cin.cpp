#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

void initialize(int argc, char** argv)
{
	if (argc < 2) 
	{
		printf("File name was not given by arguments\n");
		exit(1);
	}

	int closed = close(0);
	if (closed == -1) 
	{
		std::cerr << "Cloulnt close stdout" << std::endl;
		exit(errno);
	}

	int file = open(argv[1], O_RDONLY);
	if (file == -1) 
	{
		printf("Something went wrong while opening the file. Error %s\n", strerror(errno));
		exit(errno);
	}
}

int main(int argc, char** argv)
{
	// initialize the program based on the requirements
	initialize(argc, argv);

	// read the string 
	std::string input;
	std::cin >> input;

	std::string reversed(input.rbegin(), input.rend());
	
	// print to the console
	std::cout << reversed << std::endl;

	return 0;
}
