#include "../include/simulator.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void append_list(struct list *ls, struct msg *message){
	list_node *n  = malloc(sizeof(struct list_node));
	if (n == NULL) 
	{printf("list is NULL\n");return;}
	n->next =NULL;
    memcpy(n->message.data ,message->data,20);

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

int sum_checksum(struct pkt *p){
	if (p ==NULL) return 0;
	int total = 0;
	for (int i = 0 ; i < 20; i++){
		total += (unsigned char)p->payload[i];	
	}
	return total + p->seqnum + p->acknum;
}
