#include "../include/simulator.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void append_list(struct list *ls, struct msg *message){
	list_node *n  = calloc(1,sizeof(struct list_node));
	if (n == NULL) 
	{printf("list is NULL\n");return;}


	if (strlen(message->data) <= 20) {
		strcpy(n->message->data,message->data);
	}else {
		printf("too big Message");
		exit(-1);
	}
	if (ls->back == NULL) {
		ls->front = n;
		ls->back = n;
		return;
	}
	ls->back->next = n;
	ls->back = n;
}


list_node *pop_list(struct list *ls){
	
	if (ls->front == NULL) return NULL;
	list_node *front = ls->front;
	ls->front = ls->front->next;

	if (ls->front == NULL) {
		ls->back = NULL;
	}
	return front; 
}	
