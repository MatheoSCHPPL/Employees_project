#include <stdlib.h>
#include <stdio.h>
#include "file.h"
#include "common.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int create_db_file(char *filePath){
	//Check if the file already exist
	int db_fd = open(filePath, O_RDWR);
	if(db_fd != -1){
		close(db_fd);
		printf("%s already exist\n",filePath);
		return STATUS_ERROR;
	} 

	//Check error while creating file 
	db_fd = open(filePath, O_RDWR | O_CREAT, 0644);
	if(db_fd == -1){
		perror("open");
		return STATUS_ERROR;
	}

	return db_fd;
} 

int open_db_file(char *filePath){
	int db_fd = open(filePath, O_RDWR);
	//Check error while opening file
	if(db_fd == -1){
		perror("open");
		return STATUS_ERROR;
	}
	return db_fd;
} 
