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

list ls; 

int nil = 0
int  sideA = 0
int  sideB = 1
/* timeout for the timer */
int ticker = 30.0
int duration = 1.0
int ackNum = 111

/*All function declarations*/
int calcCkSm(struct packet *pk);
void msgAppend(struct msgs *msg);
struct node *msgDelete();
/*end*/

/*buffer*/
struct node {
  struct msgs msg;
  struct node *next;
};
struct node *list_head = nil;
struct node *list_end = nil;
/*end*/

struct sr_window{
  struct packet pi;//packet item
  int ackNum;
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
int A_seqnum = 0;
int B_seqnum = 0;
int win=0;
int temp=0;
float curTime=0;
int waitng_ack=0;
bool timerOff=TRUE;

/******************************Helper Functions*********************************/
int calcCkSm(struct packet *pk){
  int ckSm = 0;
  if(pk != nil){
    for (int i=0; i<20; i++){
      ckSm += (unsigned char)pk->payload[i];
    }
    ckSm += pk->seqN;
    ckSm += pk->ackN;
  }
  return ckSm;
}

// void msgAppend(struct msgs *msg){
//   /*allocate memory*/
//   struct node *newNode = malloc(sizeof(struct node));
//   if(newNode != nil) {
//     newNode->next = nil;
//     /*copy packet*/
//     for(int i = 0; i < 20; ++i) {
//       newNode->msg.data[i] = msg->data[i];
//     }
//     /* if list empty, just add into the list*/
//     if(list_end != nil){
//       /* otherwise, add at the end*/
//       list_end->next = newNode;
//       list_end = newNode;
//     }
//     else{
//       list_head = newNode;
//       list_end = newNode;
//       return;
//     }
//   }
//   else{
//     printf("no enough memory\newNode");
//     return;
//   }
// }

// struct node *msgDelete(){
//   struct node *pk;
//   /* if the list is empty, return nil*/
//   if(list_head != nil) {
//     /* retrive the first node*/
//     pk = list_head;
//     list_head = pk->next;
//     if(list_head == nil) {
//       list_end = nil;
//     }
//     return pk;
//   }
//   return nil;
// }

/* called from layer 5, passed the data to be sent to other side */
void A_output(msg)
  struct msgs msg;
{
  struct node *newNode;
  append_list(&ls, &msg);
  // check if the window is full
  if(pkt_in_window != win){
    n = pop_list(%ls);
    if(n != NULL){
      if(((last+1)%win)!=window_start){
        if(pkt_in_window!=0){
          last=(last+1)%win;
        }
        else{
          A_packets[last];//the selected packet of the window
          for (int i=0; i<20; i++){
              A_packets[last].pi.payload[i] = n->message.data[i];
            }
          free(n);
          A_packets[last].pi.seqnum = A_seqnum;
          A_packets[last].pi.acknum = ackNum;
          A_packets[last].pi.checksum = calcCkSm(&A_packets[last].pi);
          A_seqnum++;
          A_packets[last].timeover=curTime+TIMEOUT;
          A_packets[last].ackNum=0;//set ackNum to not received
          pkt_in_window++;//increase the number of packets in the window
          printf("sending seq no:%d\n",A_packets[last].pi.seqnum);
          tolayer3(A, A_packets[last].pi);
          if(timerOff){
            timerOff=FALSE;
            printf("Timer on\n");
            starttimer(A,INTERVAL);
          }
          return 0;
        }
      }
      else{
        return;
      }
    }
    else{
      return 0;
    }
  }
  return;
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct packet packet;
{

}

/* called when sideA's timer goes off */
void A_timintrupt(){
  curTime += duration;
  if(pkt_in_window != 0){
    int i=window_start;
    while(i!=last){
      if(A_packets[i].ackNum==0&& A_packets[i].timeover<curTime){
        printf("sending seq no:%d\newNode",A_packets[i].pi.seqN);
        A_packets[i]->timeover=curTime+ticker;
        tolayer3(sideA, A_packets[i].pi);
      }
      i=(i+1)%win;
    }
    if(A_packets[i].ackNum==0&& A_packets[i].timeover<curTime){
        printf("sending seq no:%d\newNode",A_packets[i].pi.seqN);
        A_packets[i].timeover=curTime+ticker;
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
  A_packets= malloc(sizeof(struct sr_window) * win);
  for(int i=0;i<win;i++){
    A_packets[i].ackNum==0;
  }
  timerOff = ;
  starttint duration);
}

/* Note that with simplex transfer from a-to-sideB, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at sideB*/
void B_input(packet)
  struct packet packet;
{

}

/* the following rouytine will be called once (only) before any other */
/* entity sideB routines are called. You can use it to do any initialization */
void B_init()
{
  win = getwinsize();
    B_packets= malloc(sizeof(struct sr_window) * win);
    for(int i=0;i<win;i++)
    {
      B_packets[i].timeover=i;//this is the sequence number here
    }
}
