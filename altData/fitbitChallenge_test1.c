//Fitbit Coding Challenge - Moyukh Chatterjee

//TO DO:
//figure out why READBLOCKSIZE define fails
//figure out how to dynamically change file name
//test on other machine to see if stdint.h works

//#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include "customtypes.h"

#define READBLOCKSIZE 3072 //number of bytes per chunk read - adjustable - larger size minimized number of reads/smaller size accounts for micro mem limitations

uint8_t inbuff[READBLOCKSIZE]; //why is this not ok syntax?
uint8_t finalbuff[48]; //48-element buffer for processing final values
uint16_t sorted[32] = {0}; //sorted array of max values

void sorter(uint16_t dataword, uint16_t *sortlist, int bottom, int top){ //binary search algo - only runs/pushes on 32 values, should be fast
	int lower = (bottom-top)/2;
	int upper = lower+1;
	if(dataword < sortlist[bottom]){
		return sorter(dataword,sortlist,bottom,lower); //check this
	}
	if(dataword > sortlist[top]){
		return sorter(dataword,sortlist,upper,top); //check this
	}
	else{
		for(int i=0;i<bottom;i++){
			sortlist[i] = sortlist[i+1]; //shift all lesser values
		}
		sortlist[bottom] = dataword; //insert 
	}
}

// main loop takes input file and writes data to output
void main(int argc, const char* argv[]){
	FILE *inbp; //create pointer to address binary
	inbp = fopen("test1.bin","rb");
	uint16_t readlen = fread(inbuff,1,READBLOCKSIZE,inbp);
	uint16_t outputbuff[32]; //data output buffer

	//writes final output buffer for last block
	if(readlen != READBLOCKSIZE){
		inbp = inbp-(READBLOCKSIZE-readlen-48); //move bytepointer to beginning of last 32 values
		fread(finalbuff,1,48,inbp); //read last 32 values into finalbuff
		for(int i=0;i<48;i=i+3){ //check the validity of below statements - converts 12-bit data to array of 16-bit words
			outputbuff[(i/3+1)*2-1] = (finalbuff[i]<<4)||(finalbuff[i+1]>>4);
			outputbuff[(i/3+1)*2] = ((finalbuff[i+1]<<8) && 0x0FFF)||(finalbuff[i+2]);
		}
	}

	//process and sort data in inbuff - binary sort
	for(int i=0;i<READBLOCKSIZE;i=i+3){ //check the validity of below statements - converts 12-bit data to array of 16-bit words
			sorter((inbuff[i]<<4)||(inbuff[i+1]>>4),sorted,0,31);
			sorter(((inbuff[i+1]<<8) && 0x0FFF)||(inbuff[i+2]),sorted,0,31);
		} //sort word by word into sorted list
	fclose(inbp);

	//write to output file
	FILE *outbp;
	outbp = fopen("output.out","w");
	fprintf(outbp,"--Sorted Max 32 Values--");
	for(int i=0;i<16;i++){
		char sortedstr[3];
		sprintf(sortedstr, "%i \n", sorted[i]);
		fprintf(outbp, "%s\n"); //newline
		fprintf(outbp,sortedstr); //cast sorted[i] to string first
	}

	fprintf(outbp, "%s\n"); //newline
	fprintf(outbp,"--Last 32 Values--");
	for(int i=0;i<16;i++){
		char outstr[3];
		sprintf(outstr, "%i \n", outputbuff[i]);
		fprintf(outbp, "%s\n"); //newline
		fprintf(outbp,outstr); //cast outputbuff to string first
	}
	fclose(outbp);	
}

