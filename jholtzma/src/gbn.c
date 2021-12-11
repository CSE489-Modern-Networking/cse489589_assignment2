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
#include "../include/simulator.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include <stddef.h>

list ls; 
struct pkt currentPacketInfer;
struct pkt *packets;

int sideA = 0
int sideB = 1
int winSize = 0; 
int winPacketInterval =0;
int last = 0;
int next = 0;
int start = 0;
int seqAckB = 0;

void A_output(message)
  struct msg message;
{

	append_list(&ls,&message);
	if (winPacketInterval == winSize) {
		printf("FULL");
		return;
	} 
	
	list_node *n = pop_list(&ls);
	if (n == NULL) {
		printf("message is NULL");
		return;
	}
	if ((last+1)%winSize == start) {
		return;
	}else if(winPacketInterval != 0) {
		last = (last + 1) % winSize;
	}
	memcpy(packets[last].payload, n->message.data,20);
	//packets[last].payload[19] = n->message.data[19];
	free(n);
	packets[last].seqnum = next;
	packets[last].acknum = 0;
	packets[last].checksum  = sum_checksum(&packets[last]); 
	next++;

	winPacketInterval++;
	tolayer3(sideA,packets[last]);
	if (start == last){
		starttimer(sideA,30);
	}
	return  ;	
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{
	if (packet.checksum != sum_checksum(&packet))  {
		return;
	}
	if(packet.acknum != packets[start].seqnum) {
		return;
	}
	packets[start].seqnum = -1;
	stoptimer(sideA);
	winPacketInterval--;
	if (winPacketInterval == 0) {
		list_node *n = pop_list(&ls);
		while(n!=NULL){
				
			memcpy(packets[last].payload,n->message.data,20);
			free(n);
			packets[last].seqnum = next;
			packets[last].acknum = 0;
			packets[last].checksum = sum_checksum(&packets[last]);
			next++;
		}
	}
	else {
		start = (start+1)%winSize; 
		list_node *n = pop_list(&ls);
		if (n!=NULL){
			last = (last + 1) %winSize;
			//packets[last];
			memcpy(packets[last].payload,n->message.data,20);
			free(n);

			packets[last].seqnum = next;
			packets[last].acknum = 0;
			packets[last].checksum = sum_checksum(&packets[last]);
			next++;

			winPacketInterval++;
			tolayer3(sideA, packets[last]);
		}
	}
	if (start != last || winPacketInterval == 1) {
		starttimer(sideA,30);
	}
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
    int i ;
    for ( i  =start;i != last; i=(i+1)%winSize )
      {
            tolayer3(sideA, packets[i]);
      }
     tolayer3(sideA, packets[i]);
 
    if(start != last || winPacketInterval==1)
      {
        starttimer(sideA, 30);
      }
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
	winSize = getwinsize();	
	packets = calloc(winSize , sizeof(struct pkt));
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
  struct pkt packet;
{
	if(packet.checksum != sum_checksum(&packet)) {
		return;
	}
	if (packet.seqnum == seqAckB) {
		++seqAckB ;
		tolayer5(B,packet.payload);
	}
	else if (packet.seqnum < seqAckB) {
		packet.acknum = packet.seqnum;
		packet.checksum = sum_checksum(&packet);
		tolayer3(sideB,packet);
	}
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
	return;
}

