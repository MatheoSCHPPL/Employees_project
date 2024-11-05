#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "parse.h"
#include "common.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

//Create a header for the new file
int create_db_header(int fd, struct Db_header **headerOut){
	//header is a pointer to store in headerOut
	struct Db_header *header = calloc(1, sizeof(struct Db_header));
	if(header == NULL){
		perror("calloc");
		return STATUS_ERROR; 
	} 

	header->version = 0x1;
	header->count = 0;
	header->magic = HEADER_MAGIC;
	header->filesize = sizeof(struct Db_header);

	//store the new header in the out pointer
	*headerOut = header;

	//return a status code, not the header
	return STATUS_SUCCESS;
} 


int validate_db_header(int fd, struct Db_header **headerOut){
	if(fd < 0){
		printf("Bad fd from the user\n");
		return STATUS_ERROR;
	} 
	
	struct Db_header *header = calloc(1, sizeof(struct Db_header));
	if(header == NULL){
		perror("calloc");
		return STATUS_ERROR;
	} 

	if(read(fd, header, sizeof(struct Db_header)) != sizeof(struct Db_header)){
		perror("read");
		return STATUS_ERROR;
	}  

	//unpack binary data from the file in the ndn type of the sytstem and store the value in the header struct
	//man noths for more infos
	header->version = ntohs(header->version);
	header->count = ntohs(header->count);
	header->magic = ntohl(header->magic);
	header->filesize = ntohl(header->filesize);

	if(header->magic != HEADER_MAGIC){
		printf("Not the right magic\n");
		return STATUS_ERROR;
	} 

	if(header->version != 1){
		printf("Not the right version\n");
		free(header);
		return STATUS_ERROR;
	} 

	struct stat db_stat = {0};
	if(fstat(fd, &db_stat) == STATUS_ERROR){
		perror("stat");
		return STATUS_ERROR;
	} 

	if(header->filesize != db_stat.st_size){
		printf("Error: corrupted db file\n");
		free(header);
		return STATUS_ERROR;
	} 
	
	*headerOut = header;
	return STATUS_SUCCESS;
} 

int output_file(int fd, struct Db_header *header, struct Employee *employees){
	if(fd < 0){
		printf("Got a bad Fd from the user\n");
		return STATUS_ERROR;
	}

	int realCount = header->count;

	//pack the data
	header->version = htons(header->version);
	header->count = htons(header->count);
	header->magic = htonl(header->magic);
	header->filesize = htonl(sizeof(struct Db_header)+sizeof(struct Employee)*realCount);

	//man lseek
	lseek(fd, 0, SEEK_SET);
	write(fd, header, sizeof(struct Db_header));
	
	for(int i = 0; i < realCount; i++){
		employees[i].hours = htonl(employees[i].hours); 
		write(fd, &employees[i], sizeof(struct Employee)); 
	}  

	return STATUS_SUCCESS;
} 

int read_employee(int fd, struct Db_header *header, struct Employee **employeeOut){
	if(fd < 0){
		printf("Got a bd Fd from the user\n");
		return STATUS_ERROR;
	} 

	int count = header->count;
	struct Employee *employees = calloc(count, sizeof(struct Employee));	
	if(employees == NULL){
		perror("calloc");
		return STATUS_ERROR;
	} 

	read(fd, employees, count*sizeof(struct Employee));

	//unpack every numerical employee data
	for(int i = 0; i < count; i++){
		employees[i].hours = ntohl(employees[i].hours);
	} 	

	*employeeOut = employees;
	return STATUS_SUCCESS;
}  

int add_employee(struct Db_header *header, struct Employee *employees, char *addString){
	//extract data from the string
	char *name = strtok(addString, ",");
	char *addr = strtok(NULL, ",");
	char *hours = strtok(NULL, ",");

	strncpy(employees[header->count-1].name, name, sizeof(employees[header->count-1]).name);	
	strncpy(employees[header->count-1].address, addr, sizeof(employees[header->count-1]).address);	
	employees[header->count-1].hours = atoi(hours);

	return STATUS_SUCCESS;
} 

void list_employees(struct Db_header *header, struct Employee *employees){
	for(int i = 0; i < header->count; i++){
		printf("Index: %d\n",i);
		printf("\tName: %s\n",employees[i].name);
		printf("\tAddr: %s\n",employees[i].address);
		printf("\tHours: %d\n",employees[i].hours);
	} 	
} 

int update_employee(struct Db_header *header, struct Employee *employees, char *updateString){
	char *name = strtok(updateString, ",");
	int hours = atoi(strtok(NULL, ","));
	int index = -1; 

	for(int i=0; i < header->count; i++){
		if(strcmp(employees[i].name, name)==0){
			index = i;
			break;
		} 	
	}  

	if(index == -1){
		return STATUS_ERROR;
	} 
	employees[index].hours = hours;
	return STATUS_SUCCESS;
} 

int delete_employee(struct Db_header *header, struct Employee **employees, char *deleteName){
	if(header->count < 1){
		printf("Db is empty\n");
		return STATUS_ERROR;
	} 

	int newCount = header->count--;
	struct Employee *newEmployees = calloc(newCount, sizeof(struct Employee));
	struct Employee *copyEmployees = *employees;
	bool found = false;

	if(newEmployees == NULL){
		printf("Error Calloc newEmployees\n");
		return STATUS_ERROR;
	} 

	if(copyEmployees == NULL){
		printf("Copy employees is Null\n");
		return STATUS_ERROR;
	} 

	//find the delete name
	for(int i = 0; i <= header->count; i++){
		if(strcmp(copyEmployees[i].name, deleteName) == 0){
			found = true;
		} 
	} 

	if(!found){
		printf("Name not found\n");
		return STATUS_ERROR;
	} 


	int index = 0;
	for(int i=0; i <= header->count; i++){
		if(strcmp(copyEmployees[i].name, deleteName) != 0){
			printf("in\n");
			strncpy(newEmployees[index].name, copyEmployees[i].name, sizeof(newEmployees[index].name));	
			strncpy(newEmployees[index].address, copyEmployees[i].address, sizeof(newEmployees[index].address));	
			newEmployees[index].hours = copyEmployees[i].hours;
			index++; 
		}else{
			printf("out\n");
		} 
	}

	header->count = newCount;
	header->filesize = sizeof(struct Db_header) + sizeof(struct Employee) * header->count;
	*employees = newEmployees;
	free(copyEmployees);
	return STATUS_SUCCESS;
} 
