/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/08/2021 11:33:12 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "../include/simulator.h"
#include "string.h"
#include "stdio.h"

int main(){
	list *value=malloc(sizeof(struct list));
	struct msg mes;
	
	strcpy(mes.data,"yo dog");
	append_list(value,&mes);
	list_node *top = pop_list(value);
	printf("value %s\n",top->message.data);
	return 1;
}
