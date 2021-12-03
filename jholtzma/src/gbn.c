#include "../include/simulator.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#define DEFAULT_ACK 111

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

/* called from layer 5, passed the data to be sent to other side */
list ls; 

struct pkt cur_pack;
struct pkt *packets;

int window_size = 0; 
int packets_in_window;
int last;
int waiting_ack;


int A_seqnum = 0;
int B_seqnum = 0;

int checksum = 0; 
void A_output(message)
  struct msg message;
{
	append_list(&ls,&message);
	if (packets_in_window == window_size) {
		printf("FULL");
		return;
	} 
	
	list_node *n = pop_list(&ls);
	if (n == NULL) {
		printf("message is NULL");
	}
	strcpy(cur_pack.payload,n->message->data);
	free(n);
	cur_pack.seqnum = A_seqnum;
	cur_pack.acknum = DEFAULT_ACK;
	cu

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{

}

/* called when A's timer goes off */
void A_timerinterrupt()
{

}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{

}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
  struct pkt packet;
{

}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{

}
