/********************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from sideA to sideB). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
#include "../include/simulator.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdbool.h"

#define A  0
#define B  1
#define ACK 111
#define TIMEOUT 30.0

#define TICKER  30.0

list ls; 

int nil = 0;
int  sideA = 0;
int  sideB = 1;
/* timeout for the timer */
int duration = 1.0;
int ackNum = 111;


/*end*/




struct sr_window{
  int ackNum;

  struct pkt pi;//packet item
  int timeover;
};
struct sr_window *A_packets;
struct sr_window *B_packets;
int pkt_in_window=0;
int pkt_in_window_B=0;
int window_start = 0;
int window_start_B = 0;
int last=0;
int last_B=0;

//sequence numbers
int sequence_A = 0;
int sequence_B = 0;
int win=0;
int temp=0;
float curTime=0;
int waitng_ack=0;
bool timerOff=true;



/* called from layer 5, passed the data to be sent to other side */
void A_output(msg)
     struct msg msg;
{
  
  append_list(&ls, &msg);
  // check if the window is full
  if(pkt_in_window != win){
    struct list_node *n = pop_list(&ls);
    if(n != NULL){
      if(((last+1)%win)!=window_start){
        if(pkt_in_window!=0){
            last=(last+1)%win;
        }
	  	memcpy(A_packets[last].pi.payload, n->message.data,20);
	  	free(n);
	  	A_packets[last].pi.seqnum  = sequence_A;
	  	A_packets[last].pi.acknum = ackNum;
	  	A_packets[last].pi.checksum = sum_checksum(&A_packets[last].pi);
	  	sequence_A++;
	  	A_packets[last].timeover=curTime+TICKER;
	  	A_packets[last].ackNum=0;//set ackNum to not received
	  	pkt_in_window++;//increase the number of packets in the window
	  	tolayer3(A, A_packets[last].pi);
	  	if(timerOff == true){
	  	  timerOff=false;
	  	  starttimer(A,duration);
	  	}
		 
      }
    }
  }
}
     
        
void set_packet(struct pkt *packet, 
				int seqnum,
				int acknum){
		packet->seqnum = seqnum;
		packet->acknum = acknum;
		packet->checksum = sum_checksum(packet);
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{
	if(packet.checksum == sum_checksum(&packet)){
		if (packet.acknum == A_packets[window_start].pi.seqnum){
			A_packets[window_start].ackNum=1;
			pkt_in_window--;
			if (pkt_in_window == 0) {
				window_start = (window_start+1)%win;
				last = (last+1 % win);
				if (ls.front != NULL) {
					struct  sr_window *sr = &A_packets[last];
					
					
					set_packet(&sr->pi,sequence_A,ACK);
					//set_packet(&sr->pi,sequence_A,ACK);
					sequence_A++;
					sr->timeover = curTime + TIMEOUT; 
					sr->ackNum = 0;
					pkt_in_window++;
					tolayer3(A,sr->pi);
				}else{

					timerOff = true;
					stoptimer(A);
				}
			}else {
			int i =window_start;	
			while(i!=last){
				int new_index = (i+1) % win;
				if (A_packets[new_index ].ackNum != -1){
					break;	
				}
				pkt_in_window--;	
				i=new_index;
//				if (i==last) {
//					last = i;
//				}
			}
			window_start = (i+1)%win;
			if (!pkt_in_window) {last = window_start;}
			struct list_node *n = pop_list(&ls);
			if (n!=NULL){
				struct  sr_window *last_pck = &(A_packets[last]);
				memcpy(last_pck->pi.payload,n->message.data,20);
				free(n);	
				set_packet(&last_pck->pi,sequence_A,ACK);
				last_pck->ackNum =0;	
				last_pck->timeover=curTime+TICKER;
				pkt_in_window++;
				tolayer3(A,last_pck->pi);
			}
			}
		}
		else if(packet.acknum > A_packets[window_start].pi.seqnum) {
			int i = 0;
			while (i!=last) {
				struct sr_window *cur_pack = &A_packets[(i+1)+win];		
				if (cur_pack->pi.seqnum == i ) { 
					cur_pack->ackNum = 1; 
					break;
				}
				i = (i + 1) % win;
			}
		}
		else if(packet.acknum <= A_packets[window_start].pi.seqnum){
				
		}
  }

}

/* called when sideA's timer goes off */
void A_timerinterrupt(){

        printf("\n================================ Inside A_timerinterrupt===================================\n");
  curTime += duration;
  if(pkt_in_window != 0){
    int i=window_start;
    while(i!=last){
      if(A_packets[i].ackNum==0&& A_packets[i].timeover<curTime){

       // printf("sending seq no:%d\newNode",A_packets[i].pi.seqN);
        A_packets[i].timeover=curTime+TICKER;
        tolayer3(sideA, A_packets[i].pi);
      }
      i=(i+1)%win;
    }
    if(A_packets[i].ackNum==0&& A_packets[i].timeover<curTime){
       // printf("sending seq no:%d\newNode",A_packets[i].pi.seqN);
        A_packets[i].timeover=curTime+TICKER;
        tolayer3(sideA, A_packets[window_start].pi);
    }
  }
  starttimer(sideA, duration);
} 

/* the following routine will be called once (only) before any other */
/* entity sideA routines are called. You can use it to do any initialization */
void A_init()
{
  win = getwinsize();
  A_packets= calloc(win,sizeof(struct sr_window));
  for(int i=0;i<win;i++){
    A_packets[i].ackNum==0;
  }
  timerOff = false;
  starttimer(0,duration);
}

/* Note that with simplex transfer from a-to-sideB, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at sideB*/
void B_input(packet)
  struct pkt packet;
{
  if(packet.checksum == sum_checksum(&packet)){
    if(packet.seqnum != sequence_B){
      if(packet.seqnum>sequence_B){
        if(packet.seqnum <= sequence_B + win){ 
          for(int m=0; m < win;m++){
            if(B_packets[m].timeover==packet.seqnum){
              B_packets[m].pi=packet;
              packet.acknum = packet.seqnum ; 
              packet.checksum = sum_checksum(&packet);
              tolayer3(B, packet);
              break;
            }
          }
        }
      }
      else{
        packet.acknum = packet.seqnum ; 
        packet.checksum = sum_checksum(&packet);
        tolayer3(B, packet);
      }
    }
    else{
      sequence_B += 1;
      tolayer5(B, packet.payload);
      packet.acknum = sequence_B-1; /* resend the latest ACK */
      packet.checksum = sum_checksum(&packet);
      tolayer3(B, packet);
      
      B_packets[window_start_B].timeover= (sequence_B) + win-1;
      window_start_B =( window_start_B + 1)%win;

      while(B_packets[window_start_B].pi.seqnum == sequence_B){
        tolayer5(B, B_packets[window_start_B].pi.payload);
        sequence_B++;
        B_packets[window_start_B].timeover=(sequence_B)+win-1;
        window_start_B=(window_start_B+1)%win;
      }
    }
  }
  else{
    printf("Packet is corrupted");
    return;
  }
}

/* the following rouytine will be called once (only) before any other */
/* entity sideB routines are called. You can use it to do any initialization */
void B_init()
{
  win = getwinsize();
    B_packets= calloc(win,sizeof(struct sr_window) );
    for(int i=0;i<win;i++)
    {
      B_packets[i].timeover=i;//this is the sequence number here
    }

}
