
#include "../include/simulator.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
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

/********* STUDENTS WRITE THE NEXT SIX ROUTINES *********/

/* called from layer 5, passed the data to be sent to other side */


 
struct pkt cur_pack;
struct list ls;
int window_start = 0;
int sequence_A; 
int IsWaiting_pkt = 0; 
int IsWaiting_ack = 1; 
int state = 0;
int sequence_B; 
#define rtt 20
void A_output(message)
     struct msg message;
{
      
  append_list(&ls,&message);
  if(state  == IsWaiting_pkt){
    state = IsWaiting_ack;   
    struct list_node *n = pop_list(&ls); 

    memcpy(cur_pack.payload,n->message.data,20);
    free(n);
    cur_pack.seqnum = sequence_A;
    cur_pack.checksum = sum_checksum(&cur_pack); 
    tolayer3(0,cur_pack); 
    starttimer(0,rtt);
  } 
  
  

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{
 
  if(packet.checksum == sum_checksum(&packet)) {
    if(packet.acknum == sequence_A){
      sequence_A = (sequence_A == 0)? 1: 0; 
      state = IsWaiting_pkt; 
    }

  }
  
}

/* called when A's timer goes off */
void A_timerinterrupt()
{

  tolayer3(0, cur_pack); 
  starttimer(0,rtt); 
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  sequence_A = 0; 
  
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
  struct pkt packet;
{
 

  if(packet.checksum == sum_checksum(&packet)  )
    {
      if(packet.seqnum == sequence_B){
	sequence_B = (sequence_B == 0)? 1:0; 
	tolayer5(1, packet.payload); 
      }
      packet.acknum = packet.seqnum; 
      packet.checksum = sum_checksum(&packet); 
      tolayer3(1,packet);
    }
 
   
}

/* the following routine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
  sequence_B = 0; 
}
