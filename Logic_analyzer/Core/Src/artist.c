/*
 * artist.c
 *
 *  Created on: Nov 4, 2025
 *      Author: lmgault22
 */


#include "artist.h"
#include "SoftwareSPI.h"
#include "queue.h"

extern Queue SPI_q;

void draw(){
	uint16_t clk[8] = {0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01};
	uint16_t data[8] = {0x80,0x80,0x80,0x01,0x80,0x80,0x01,0x01};
	//static uint16_t oldData[8] = {0x00,0x80,0x80,0x01,0x80,0x80,0x01,0x01};

	/*if(oldData[0] != data[0]){
		reset();
		oldData[0] = data[0];
	}*/


	//sendSPI_GRAPH(0xB0, false);
	enqueue(&SPI_q, 0x010000B0);
		  for(int i = 0; i<3; i++){
			  //sendSPI_GRAPH(0x01, true);
			  enqueue(&SPI_q, 0x01100010);
		  }
		  //sendSPI_GRAPH(0xFF, true);
		  enqueue(&SPI_q, 0x011000FF);
		  for(int n = 0; n<2; n++){
		  	  for(int i = 0; i<8; i++){
		  		  for(int j = 0; j<4; j++){
		  			  //sendSPI_GRAPH(clk[i], true);
		  			  enqueue(&SPI_q, 0x01100000 | clk[i]);
			  	  }
		  		  if((i==7)&&(n==1)){
		  			  break;
		  		  }
		  		  //sendSPI_GRAPH(0xFF, true);
		  		  enqueue(&SPI_q, 0x011000FF);
		  	  }
		  }
		  for(int i = 0; i<35; i++){
			  //sendSPI_GRAPH(0x01, true);
			  enqueue(&SPI_q, 0x01100001);
		  }
		  //sendSPI_GRAPH(0xB3, false);
		  enqueue(&SPI_q, 0x010000B3);
		  for(int i = 0; i<3; i++){
			  //sendSPI_GRAPH(0x01, true);
			  enqueue(&SPI_q, 0x01100001);
		  }
		  //sendSPI_GRAPH(0xFF, true);
		  enqueue(&SPI_q, 0x011000FF);
		  for(int n = 0; n<2; n++){
		  	  for(int i = 0; i<8; i++){
		  		  for(int j = 0; j<4; j++){
				  	  //sendSPI_GRAPH(data[i], true);
		  			  enqueue(&SPI_q, 0x01100000 | data[i]);
			  	  }
		  		  if((i==7)&&(n==1)){
		  			  break;
		  		  }
		  		  if(i>0){
		  		  	  if(data[i]!=data[i+1]){
		  		  		  //sendSPI_GRAPH(0xFF, true);
		  		  		  enqueue(&SPI_q, 0x011000FF);
		  		  	  }
		  		  }
		  	  }
		  }
		  for(int i = 0; i<35; i++){
			  //sendSPI_GRAPH(0x01, true);
			  enqueue(&SPI_q, 0x01100001);
		  }
		  //sendSPI_GRAPH(0xB6, false);
		  enqueue(&SPI_q, 0x010000B6);
		  for(int i = 0; i<3; i++){
			  //sendSPI_GRAPH(0x80, true);
			  enqueue(&SPI_q, 0x01100080);
		  }
		  //sendSPI_GRAPH(0xFF, true);
		  enqueue(&SPI_q, 0x011000FF);
		  for(int i = 0; i<74; i++){
			  //sendSPI_GRAPH(0x01, true);
			  enqueue(&SPI_q, 0x01100001);
		  }
		  //sendSPI_GRAPH(0xFF, true);
		  enqueue(&SPI_q, 0x011000FF);
		  for(int i = 0; i<25; i++){
			  //sendSPI_GRAPH(0x80, true);
			  enqueue(&SPI_q, 0x01100080);
		  }
}
