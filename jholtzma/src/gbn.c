#include "../include/simulator.h"


#define NULL 0

#define TIMEOUT 30.0

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#define DEFAULT_ACK 111
#define A 0
#define B 1

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
int packets_in_window =0;
int last = 0;
int waiting_ack =0 ;
int nextseq = 0;

int window_start = 0;

int B_seqnum = 0;

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
		return;
	}
	if ((last+1)%window_size == window_start) {
		return;
	}else if(packets_in_window != 0) {
		last = (last + 1) % window_size;
	}
	memcpy(packets[last].payload, n->message.data,20);
	//packets[last].payload[19] = n->message.data[19];
	free(n);
	packets[last].seqnum = nextseq;
	packets[last].acknum = DEFAULT_ACK;
	packets[last].checksum  = sum_checksum(&packets[last]); 
	nextseq++;

	packets_in_window++;
	tolayer3(A,packets[last]);
	if (window_start == last){
		starttimer(A,TIMEOUT);
	}
	return 0 ;	
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{
	if (packet.checksum != sum_checksum(&packet))  {
		return;
	}
	if(packet.acknum != packets[window_start].seqnum) {
		return;
	}
	packets[window_start].seqnum = -1;
	stoptimer(A);
	packets_in_window--;
	if (packets_in_window == 0) {
		list_node *n = pop_list(&ls);
		while(n!=NULL){
				
			memcpy(packets[last].payload,n->message.data,20);
			free(n);
			packets[last].seqnum = nextseq;
			packets[last].acknum = DEFAULT_ACK;
			packets[last].checksum = sum_checksum(&packets[last]);
			nextseq++;
		}
	}
	else {
		window_start = (window_start+1)%window_size; 
		list_node *n = pop_list(&ls);
		if (n!=NULL){
			last = (last + 1) %window_size;
			//packets[last];
			memcpy(packets[last].payload,n->message.data,20);
			free(n);

			packets[last].seqnum = nextseq;
			packets[last].acknum = DEFAULT_ACK;
			packets[last].checksum = sum_checksum(&packets[last]);
			nextseq++;

			packets_in_window++;
			tolayer3(A, packets[last]);
		}
	}
	if (window_start != last || packets_in_window == 1) {
		starttimer(A,TIMEOUT);
	}
}

/* called when A's timer goes off */
void A_timerinterrupt()
{

printf("\n================================ A_timerinterrupt===================================\n");
	int i  =window_start;
	while ( i != last  )
  	{
		printf("sending seq no:%d\n",packets[i].seqnum);
  	  	tolayer3(A, packets[i]);
		i=(i+1)%window_size;
  	}
   	printf("sending seq no:%d\n",packets[i].seqnum);
 	tolayer3(A, packets[i]);
 
	if(window_start != last || packets_in_window==1)
  	{
		starttimer(A, TIMEOUT);
  	}
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
	window_size = getwinsize();	
	packets = malloc(window_size * sizeof(struct pkt));
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
  struct pkt packet;
{
	if(packet.checksum != sum_checksum(&packet)) {
		printf("packed_error");
		return;
	}
	if (packet.seqnum == B_seqnum) {
		++B_seqnum ;
		tolayer5(B,packet.payload);
	}
	else if (packet.seqnum < B_seqnum) {
		packet.acknum = packet.seqnum;
		packet.checksum = sum_checksum(&packet);
		tolayer3(B,packet);
	}
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{

}
