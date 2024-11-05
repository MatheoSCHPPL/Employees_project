#ifndef PARSE_H
#define PARSE_H

#define HEADER_MAGIC 0x5a495a49

struct Db_header{
	unsigned int magic;//controle value
	unsigned short version;
	unsigned short count;
	unsigned int filesize;
}; 

struct Employee{
	char name[256];
	char address[256];
	unsigned int hours;
}; 

//The fd is the intput and the pointer to pointer is the output, the return is a status code
int create_db_header(int, struct Db_header **);
int validate_db_header(int, struct Db_header **);
int output_file(int, struct Db_header *, struct Employee *);
int read_employee(int, struct Db_header *, struct Employee **); 
int add_employee(struct Db_header *, struct Employee *, char *);
void list_employees(struct Db_header *, struct Employee *);
int update_employee(struct Db_header *, struct Employee *, char *);
int delete_employee(struct Db_header *, struct Employee **, char *);
#endif
