#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include "file.h"
#include "parse.h"
#include "common.h"

//Print the flags usage
void print_usage(char *argv[]){
	printf("Usage: %s -n -f <database file>\n",argv[0]);
	printf("\t -n - Create new file\n");
	printf("\t -f - (required) path to database\n");
	return;
}  

int main(int argc, char *argv[]){
	char *pathFile = NULL;	
	char *addString = NULL;
	char *newHour = NULL;
	char *deleteName = NULL;
	bool newFile = false;	
	bool list = false;
	int c;
	int db_fd = -1;
	struct Db_header *header = NULL;
	struct Employee *employees = NULL;

	while((c = getopt(argc,argv, "nf:a:lu:d:")) != -1){
		switch(c){
			case 'n':
				newFile = true;
				break;
			case 'f':
				pathFile = optarg;//optarg = pointer to the cli value
				break;
			case 'a':
				addString = optarg;
				break;
			case 'l':
				list = true;
				break;
			case 'u':
				newHour = optarg;
				break;
			case 'd':
				deleteName = optarg;
				break;
			case '?':
				printf("invalide flag\n");
				break;
			default:
				return -1;
		}  	
	} 
	
	//Check for the file path
	if(pathFile == NULL){
		printf("Path file is required \n");
		print_usage(argv);
	}


	//Check if is a new file
	if(newFile){
		db_fd = create_db_file(pathFile);
		if(db_fd == STATUS_ERROR){
			printf("Unable to create database file\n");
			return STATUS_ERROR;
		} 

		//check for the db header
		if(create_db_header(db_fd, &header) == STATUS_ERROR){
			printf("Failed to create db header\n");
			return STATUS_ERROR;
		} 

	}else{//Check for open an already exist file
		db_fd = open_db_file(pathFile);
		if(db_fd == STATUS_ERROR){
			printf("Unable to create database file\n");
			return STATUS_ERROR;
		} 

		if(validate_db_header(db_fd, &header) == STATUS_ERROR){
			printf("Failed to read the db header\n");
			return STATUS_ERROR;
		} 
	}  

	//Read employees data and store it in employees
	if(read_employee(db_fd, header, &employees) == STATUS_ERROR){
		printf("Can't read employee\n");
		return STATUS_ERROR;
	}  

	//add an employee
	if(addString){
		header->count++; 
		employees = realloc(employees, sizeof(struct Employee)*header->count);
		add_employee(header, employees, addString);
	}

	//update hour
	if(newHour){
		if(update_employee(header, employees, newHour) == STATUS_ERROR){
			printf("Invalid name\n");
			return STATUS_ERROR;
		} 
	} 
	
	if(deleteName){
		if(delete_employee(header, &employees, deleteName) == STATUS_ERROR){
			printf("can't delete\n");
			return STATUS_ERROR;
		} 
	} 

	//list all the employees
	if(list){
		list_employees(header, employees);
	} 	
	printf("new file: %d\n",newFile);
	printf("path: %s\n",pathFile);

	//use xxd <db_filename>
	output_file(db_fd, header, employees);

	return 0;

}   
