#include "../include/simulator.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
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
#define true 1
#define false 0


int sequence_A, sequence_B; 
bool IsWaiting_pkt, IsWaiting_ack, state;  
struct pkt cur_pack;
struct pkt *packets; 
#define CHUNKSIZE 20
struct list ls;
int window_start = 0;
#define DEFAULT_ACK 111;
void A_output(message)
  struct msg message;
{
      
  append_list(&ls,&message);
  list_node *n = pop_list(&ls); 
  if(state  == IsWaiting_pkt && n != NULL){
        
    strncpy(cur_pack.payload,n->message->data,20);
    cur_pack.seqnum = sequence_A;
    cur_pack.acknum = DEFAULT_ACK;
    cur_pack.checksum = sum_checksum(&cur_pack); 
    tolayer3(0,cur_pack); 
    starttimer(0,20);
  }
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{
 
  if(packet.acknum == sequence_A) {
    IsWaiting_ack = true; 
    stoptimer(0);
    sequence_A ++;
  }
  else{
    starttimer(0,20); 
    tolayer3(0,cur_pack); 
  }
  
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
  tolayer3(0, cur_pack); 
  starttimer(0,20); 
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  sequence_A = 0; 
  IsWaiting_pkt = false; 
  IsWaiting_ack = true; 
  state = false;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
  struct pkt packet;
{
   if(packet.checksum != sum_checksum(&packet))
  {
    return;
  }

  if(packet.seqnum == sequence_B)
  {
    sequence_B = (sequence_B + 1 ) % 2; 
    tolayer5(1, packet.payload); 
  }
  packet.acknum = packet.seqnum; 
  packet.checksum = sum_checksum(&packet); 
  tolayer3(1,packet); 
}

/* the following routine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
  sequence_B =0; 
}
