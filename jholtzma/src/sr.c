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
double timeOut = 30.0;
int sideA = 0;
int sideB = 1;
int duration = 1.0;
int ackNum = 111;
int pkInAWin=0;
int stWinA = 0;
int stWinB = 0;
int LST = 0;
int Aseqc = 0;
int Bseqc = 0;
int win = 0;
float curTime = 0;
bool timerOff = true;

struct winSR{
  int ackNum;
  struct pkt pi;
  int timeOver;
};
struct winSR *pktA;
struct winSR *pktB;

/* called from layer 5, passed the data to be sent to other side */
void A_output(msg)
struct msg msg;
{
  append_list(&ls, &msg);
  if(pkInAWin != win){
    struct list_node *n = pop_list(&ls);
    if(n != NULL){
      if(((LST+1)%win)!=stWinA){
        if(pkInAWin!=0){
            LST=(LST+1)%win;
        }
	  	memcpy(pktA[LST].pi.payload, n->message.data,20);
	  	free(n);
	  	pktA[LST].pi.seqnum  = Aseqc;
	  	pktA[LST].pi.acknum = ackNum;
	  	pktA[LST].pi.checksum = sum_checksum(&pktA[LST].pi);
      A_set(&pktA[LST]);
	  	tolayer3(sideA, pktA[LST].pi);
	  	if(timerOff == true){
	  	  timerOff=false;
	  	  starttimer(sideA,duration);
	  	}
      }
    }
  }
}

void set_packet(struct pkt *packet,int seqnum,int acknum){
  packet->seqnum = seqnum;
  packet->acknum = acknum;
  packet->checksum = sum_checksum(packet);
}

void A_input(packet)
struct pkt packet; 
{
  if (packet.checksum != sum_checksum(& packet)){
    return;
  }
  else if (packet.acknum > pktA[stWinA].pi.seqnum){
    int i = stWinA;
    while (i != LST) {
      struct winSR * cur_pack = & pktA[(i + 1) + win];
      if (cur_pack -> pi.seqnum == packet.acknum) {
        cur_pack -> ackNum = 1;
        break;
      }
      i = (i + 1) % win;
    }
  }
  else if (packet.acknum == pktA[stWinA].pi.seqnum){
    pktA[stWinA].ackNum = 1;
    pkInAWin--;
    if (pkInAWin != 0){
      A_input_accumulate();
    }
    else{
      stWinA = (stWinA + 1) % win;
      LST = (LST + 1) % win;
      if (ls.front == NULL){
        timerOff = true;
        stoptimer(sideA);
      }
      else{
        struct winSR * sr = & pktA[LST];
        set_packet(&sr -> pi, Aseqc, ackNum);
        A_set(sr);
        tolayer3(sideA, sr -> pi);
      }
    }
  }
}

void A_set(struct winSR *x){
  Aseqc++;
  x -> ackNum = 0;
  x -> timeOver = curTime + timeOut;
  pkInAWin++;
}

void A_input_accumulate(){
  int i = stWinA;
  while (i != LST){
    int new_index = (i + 1) % win;
    if (pktA[new_index].ackNum != 1){
      break;
    }
    pkInAWin--;
    i = new_index;
  }
  stWinA = (i + 1) % win;
  if (pkInAWin == 0){
    LST = stWinA;
  }
  struct list_node * n = pop_list( &ls);
  if (n != NULL){
    struct winSR * last_pck = & (pktA[LST]);
    memcpy(last_pck -> pi.payload, n -> message.data, 20);
    free(n);
    set_packet( & last_pck -> pi, Aseqc, ackNum);
    set_A(last_pck);
    tolayer3(sideA, last_pck -> pi);
  }
}

/* called when sideA's timer goes off */
void A_timerinterrupt(){
  curTime += duration;
  if(pkInAWin != 0){
    int i=stWinA;
    while(i!=LST){
      if(pktA[i].ackNum==0&& pktA[i].timeOver<curTime){
        pktA[i].timeOver=curTime+timeOut;
        tolayer3(sideA, pktA[i].pi);
      }
      i=(i+1)%win;
    }
    if(pktA[i].ackNum==0&& pktA[i].timeOver<curTime){
      pktA[i].timeOver=curTime+timeOut;
      tolayer3(sideA, pktA[stWinA].pi);
    }
  }
  starttimer(sideA, duration);
}

/* the following routine will be called once (only) before any other */
/* entity sideA routines are called. You can use it to do any initialization */
void A_init(){
  init_all(0);
  timerOff = false;
  starttimer(0,duration);
}

/* Note that with simplex transfer from a-to-sideB, there is no B_output() */
/* called from layer 3, when a packet arrives for layer 4 at sideB*/
void B_input(packet)
  struct pkt packet;
{
  if(packet.checksum == sum_checksum(&packet)){
    if(packet.seqnum != Bseqc){
      if(packet.seqnum>Bseqc){
        if(packet.seqnum <= Bseqc + win){ 
          for(int m=0; m < win;m++){
            if(pktB[m].timeOver==packet.seqnum){
              pktB[m].pi=packet;
              set_B_input(packet,sum_checksum(&packet));
              break;
            }
          }
        }
      }
      else{
        set_B_input(packet,sum_checksum(&packet));
      }
    }
    else{
      Bseqc += 1;
      tolayer5(sideB, packet.payload);
      set_B_input(packet,Bseqc-1);
      pktB[stWinB].timeOver= (Bseqc) + win-1;
      stWinB =( stWinB + 1)%win;

      while(pktB[stWinB].pi.seqnum == Bseqc){
        tolayer5(sideB, pktB[stWinB].pi.payload);
        Bseqc++;
        pktB[stWinB].timeOver=(Bseqc)+win-1;
        stWinB=(stWinB+1)%win;
      }
    }
  }
}

void set_B_input(struct pkt packet, int y){
  packet.acknum = y ; 
  packet.checksum = sum_checksum(&packet);
  tolayer3(sideB, packet);
}

/* the following rouytine will be called once (only) before any other */
/* entity sideB routines are called. You can use it to do any initialization */
void B_init(){
  init_all(1);
}

void init_all(int flag){
  win = getwinsize();
  if(flag == 1){
    pktB = calloc(win,sizeof(struct winSR));
  }
  else{
    pktA = calloc(win,sizeof(struct winSR));
  }
  pktB = calloc(win,sizeof(struct winSR));
  for(int i=0;i<win;i++){
    if(flag == 1){
      pktB[i].timeOver=i;
    }
    else{
      pktA[i].ackNum==0;
    }
  }
}
