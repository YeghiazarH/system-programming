#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 4096

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		printf("Please provide source and destination");
		exit(1);
	}

	int sourceFd = open(argv[1], O_RDONLY);

	if (sourceFd == -1)
	{
		printf("Something went wrong while opening the source file. Error %s\n", strerror(errno));
		exit(errno);
	}

	int destFd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);

	if (destFd == -1)
	{
		printf("Something went wrong while opening the destination file. Error %s\n", strerror(errno));
		close(sourceFd);
		exit(errno);
	}

	char* buffer = (char*)malloc(BUFFER_SIZE);
	off_t current = 0, size = lseek(sourceFd, 0, SEEK_END), hole = 0, data = 0;
	lseek(sourceFd, 0, SEEK_SET);

	while (current < size)
	{
		off_t dataStart = lseek(sourceFd, current, SEEK_DATA);
		if (dataStart == -1)
		{
			if (errno == ENXIO)
			{
				break;
			}
			close(sourceFd);
			close(destFd);
			exit(errno);
		}

		if (dataStart > current)
		{
			lseek(destFd, dataStart, SEEK_SET);
		}

		off_t holeStart = lseek(sourceFd, dataStart, SEEK_HOLE);
		if (holeStart == -1)
		{
			close(sourceFd);
			close(destFd);
			exit(errno);
		}

		off_t copySize = holeStart - dataStart;
		data += copySize;
		lseek(sourceFd, dataStart, SEEK_SET);

		while (copySize > 0)
		{
			int readSize = read(sourceFd, buffer, copySize > BUFFER_SIZE ? BUFFER_SIZE : copySize);

			if (readSize == -1)
			{
				printf("Something went wrong while reading the file. Error %s\n", strerror(errno));
				close(sourceFd);
				close(destFd);
				exit(errno);
			}

			int wptr = 0;
			while (wptr < readSize)
			{
				int w = write(destFd, buffer + wptr, readSize - wptr);
				if (w == -1)
				{
					printf("Something went wrong while writing to destination file. Error %s\n", strerror(errno));
					close(sourceFd);
					close(destFd);
					exit(errno);
				}
				wptr += w;
			}

			copySize -= readSize;
		}

		current = holeStart;
	}

	if (size > 0)
	{
		ftruncate(destFd, size);
	}

	close(sourceFd);
	close(destFd);
	free(buffer);

	hole = size - data;
	printf("Successfully copied %lld bytes (data: %lld, hole: %lld).\n", (long long)size, (long long)data, (long long)hole);

	return 0;
}