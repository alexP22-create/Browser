/* POSTOLACHE Alexandru-Gabriel */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#ifndef _DOWNLOAD_RESOURCE_
#define _DOWNLOAD_RESOURCE_
typedef void (*TF)(void*);//free an element
typedef struct cel
{ 
	struct cel *urm; /* adr. of the next cell */
	void* info; /* adresa informatie */

} TCel, *ACel;

typedef struct stiva
{ 
	size_t dime; /* element's dimension/size */
	ACel vf; /* the adress of the cell which is on top of the queue */

} TStiva, *ASt;

typedef struct coada
{
	size_t dime;
	ACel ic, sc;
}TCoada, *AQ;

typedef struct{
    char name[100];
    unsigned long dimension;
    unsigned long currently_downloaded;
} Resource;

typedef struct web_page{
	char url[21];
	int numar_res;
	Resource *resources;
}W_P,*Web_Page;

//the structure for the list of COMPLETED DOWNLOADS
typedef struct desc_final{
struct desc_final *next;
Resource* info;
}D_F,*Desc_Final;

typedef struct tab{
	Web_Page current_page;
	ASt back_stack;
	ASt forward_stack;
	struct tab *next;//list of tabs
	struct tab *current;//current tab
	//for all the other tabs except list
	//the current field will be null
	//and for the list it will point to one of it's elements
}Tab,*TAB;

Resource* get_page_resources(const char *URL_Name, int *n);

#endif /* _DOWNLOAD_RESOURCE_ */
