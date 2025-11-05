/*
 * artist.c
 *
 *  Created on: Nov 4, 2025
 *      Author: lmgault22
 */


#include "artist.h"

void draw(){
	uint16_t clk[8] = {0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01};
	uint16_t data[8] = {0x80,0x80,0x80,0x01,0x80,0x80,0x01,0x01};

	command(0xB0);
		  for(int i = 0; i<3; i++){
			  giveData(0x01);
		  }
	  	  giveData(0xFF);
		  for(int n = 0; n<2; n++){
		  	  for(int i = 0; i<8; i++){
		  		  for(int j = 0; j<4; j++){
				  	  giveData(clk[i]);
			  	  }
		  		  if((i==7)&&(n==1)){
		  			  break;
		  		  }
			  	  giveData(0xFF);
		  	  }
		  }
		  for(int i = 0; i<35; i++){
			  giveData(0x01);
		  }
		  command(0xB3);
		  for(int i = 0; i<3; i++){
			  giveData(0x01);
		  }
		  giveData(0xFF);
		  for(int n = 0; n<2; n++){
		  	  for(int i = 0; i<8; i++){
		  		  for(int j = 0; j<4; j++){
				  	  giveData(data[i]);
			  	  }
		  		  if((i==7)&&(n==1)){
		  			  break;
		  		  }
		  		  if(i>0){
		  		  	  if(data[i]!=data[i+1]){
		  		  		  giveData(0xFF);
		  		  	  }
		  		  }
		  	  }
		  }
		  for(int i = 0; i<35; i++){
			  giveData(0x01);
		  }
		  command(0xB6);
		  for(int i = 0; i<3; i++){
			  giveData(0x80);
		  }
		  giveData(0xFF);
		  for(int i = 0; i<74; i++){
			  giveData(0x01);
		  }
		  giveData(0xFF);
		  for(int i = 0; i<25; i++){
			  giveData(0x80);
		  }
}
