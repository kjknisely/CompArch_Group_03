/**********************************
Computer Architecture Group Project 
Cache Simulator
Richard Azille
Keegan Knisely
Sabrina Mosher
**********************************/

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sim.h"


static int ADDRESS_SPACE = 32; // 32-bit address space
static int cachesize = 0, blocksize = 0, associativity = 0, replacement = -1;

int main(int argc, char *argv[]){
	int i;
	FILE *infp = NULL;
	char *infilename=NULL;
	if(argc != 11){
		printf("Usage: %s -f <trace file name> -s <cache size in KB> -b <block size> -a <associativity> -r <replacment policy(LRU,RR,RND)>\n", argv[0]);
		return -1;
	}

    // get all of the command line arguments
	for(i=1;i<argc;i++){
        // trace file name
		if(strcmp(argv[i],"-f")==0){
			i++;
			infp = fopen(argv[i], "r");
			if (infp == NULL){
				printf("Unable to open trace file %s, EXITING\n", argv[i]);
				return -2;
			}
			infilename = argv[i];
		}else if(strcmp(argv[i], "-s")==0){
            // cache size
			i++;
			cachesize = atoi(argv[i]);
			if(cachesize < 1 || cachesize > 8192){
				printf("Invalid cache size, 1KB(1) to 8MB(8192): EXITING\n");
				return -3;
			}
		}else if(strcmp(argv[i], "-b")==0){
            // block size
			i++;
			blocksize = atoi(argv[i]);
			if(blocksize < 4 || blocksize > 64){
				printf("Invalid block size, blocks are 4B->64B: EXITING\n");
				return -4;
			}
		}else if(strcmp(argv[i], "-a")==0){
            // associativity
			i++;
			associativity = atoi(argv[i]);
			if(!(associativity == 1 || associativity == 2 || associativity == 4 || associativity == 8 || associativity == 16)){
				printf("Invalid associativity rate(1 2 4 8 16): EXITING\n");
				return -5;
			}
		}else if(strcmp(argv[i], "-r")==0){
            // replacement policy
			i++;
			if(strcmp(argv[i], "RR")==0){
				replacement = 1; /*****replacement policy round robin == 1*******/
			}else if(strcmp(argv[i],"RND")==0){
				replacement = 2; /*****replacement policy is random == 3*********/
			}else if(strcmp(argv[i], "LRU")==0){
				replacement = 3; /*****replacement policy is LeastRecentlyUsed == 5****/
			}else{
				printf("Invalid replacement policy(RR, RND, LRU): EXITING\n");
				return -6;
			}
		}else {
			printf("Invalid Command Line Argument\n");
			return -7;
		}
	}

	printHeader(argv, argc, infilename);
	printCalculatedValues();
	printCacheHitRate();
	parseTrace(infp);

	fclose(infp);
	return 1;
}

/*
 * Parse the input trace file
 */
void parseTrace(FILE * traceFile)
{
    char buf[1000];
    int eAddr, eSize, wAddr, rAddr;
    int addrs[20];
    int sizes[20];
    int cnt = 0;

    // read to eof
    while (fgets(buf, 999, traceFile) != NULL)
    {
        // not EIP line
        if (buf[0] == 'E') {
            parseEipLine(buf, &eAddr, &eSize);
            // store first 20 addresses and size
            if (cnt < 20) {
                addrs[cnt] = eAddr;
                sizes[cnt] = eSize;
                cnt++;
            }
        }
        else if (buf[0] == 'd') {
            parseDataLine(buf, &wAddr, &rAddr);
            //printTraceData(eAddr, eSize, wAddr, rAddr);
        }
    }

    int i;
    // print the rest in a list
    for (i = 0; i < 20; i++)
    {
        printf("0x%08x: (%d)\n", addrs[i], sizes[i]);
    }
}

/*
 * Print the trace data using the given params.
 */
void printTraceData(int eAddr, int eSize, int wAddr, int rAddr) {
    printf("Address = 0x%08x, length = %d. ", eAddr, eSize);

    // read or write affects output
    if (wAddr == 0 && rAddr == 0)
        printf("No data writes/reads occurred.\n\n");
    else if (wAddr == 0)
        printf("No data writes occurred. Data read at 0x%08x, length = 4 bytes\n\n", rAddr);
    else if (rAddr == 0)
        printf("Data write at 0x%08x, length = 4 bytes. No data reads occured\n\n", wAddr);
    else
        printf("Data write is at 0x%08x, length = 4 bytes, data read at 0x%08x, length = 4 bytes\n\n",
            wAddr, rAddr);
}

/*
 * Parse an EIP line
 */
void parseEipLine(char * buf, int *addr, int *size)
{
    if (sscanf(buf, "EIP (%d): %x", size, addr) != 2)
    {
        perror("Invalid EIP line. Unable to read all bytes.");
        exit(-1);
    }
    return;
}

/*
 * Parse a data line
 */
void parseDataLine(char *buf, int *writeAddr, int *readAddr)
{
    *readAddr = -1;
    char dataDst[9];
    if (sscanf(buf, "dstM: %x %8s srcM: %x", writeAddr, dataDst, readAddr) < 3
        && *readAddr == -1)
    {
        perror("Invalid data line. Unable to read dest and source addresses.");
        exit(-1);
    }
    return;
}

/*
 * Print the header for the simulation.
 */
void printHeader(char *argv[], int argc, char *filename){
	int i;

	printf("Cache Simulator CS 3853 Spring 2019 - Group #3\n\n");

    	// command line arguments
	printf("Cmd Line:");
	for(i=0;i<argc;i++)
	{		
		printf("%s ",argv[i]);
	}
	printf("\n");

    	// parsed info
	printf("Trace File: %s\n", argv[2]);
	printf("Cache Size: %d KB\n", cachesize);
	printf("Block Size: %d bytes\n", blocksize);
	printf("Associativity: %d\n", associativity);
	printf("Policy: %s\n", getPolicyString(replacement));
	printf("\n");
}

/*
 * Given an integer determine the corresponding replacement
 * policy used
 */
char *getPolicyString(int replacement){
	if(replacement == 1){
		return "RR";
	}else if(replacement == 2){
		return "RND";
	}else{
		return "LRU";
	}
}

/*
 * Calculate some values based off of the given parameters
 */
void printCalculatedValues(){
	int totalBlocks = calculateTotalBlocks(cachesize, blocksize);
	int indexSize = calculateIndexSize(totalBlocks, associativity);
	int totalIndices = calculateTotalIndices(indexSize);
	int tagSize = calculateTagSize(blocksize, indexSize);
	if(totalBlocks < 1024)
	{
		printf("Total # Blocks: %d (2^%.0lf)\n", totalBlocks, (log(totalBlocks)/(log(2))));
	}else
	{
		printf("Total # Blocks: %d KB (2^%.0lf)\n", totalBlocks / 1024, log(totalBlocks/1024)/log(2)+10); 
	}
	printf("Tag Size: %d bits\n", tagSize);
	if(indexSize < 10){
		printf("Index Size: %d bits, Total Indices: %d\n", indexSize, (int)pow(2,indexSize));
	}else{
		printf("Index Size: %d bits, Total Indices: %d K\n", indexSize, totalIndices);
	}
	printf("Overhead Memory Size: %d bytes\n", calculateOverheadMemorySize(tagSize, totalBlocks));
    	printf("Implementation Memory Size: %d bytes\n", cachesize * 1024 + calculateOverheadMemorySize(tagSize, totalBlocks));
	printf("\n");
}

/*
 * Calculate the total number of blocks in the cache
 */
int calculateTotalBlocks(int cachesize, int blocksize){
	return cachesize * 1024 / blocksize;
}

/* 
 * Calculate the size of the tag
 */
int calculateTagSize(int blocksize, int indexSize){
	int blockBits = log(blocksize) / log(2);
	return ADDRESS_SPACE - blockBits - indexSize;
}

/* 
 * Calculate the size of the index
 */
int calculateIndexSize(int totalBlocks, int associativity){
	return log(totalBlocks / associativity) / log(2);
}

/* 
 * Calculate the total number of indices
 */
int calculateTotalIndices(int indexSize){
	return pow(2, indexSize) / 1024;
}

/* 
 * Calculate the impelementation's memory size
 */
int calculateOverheadMemorySize(int tagSize, int totalBlocks){
	return (int)ceil(((tagSize + 1) * totalBlocks) / 8.0);
}

/*
 * Print out the hit rate
 */
void printCacheHitRate(){
	// TODO: Actually calulate and report cache hit rate for future milestones
	printf("Cache Hit Rate: __%%\n");
	printf("\n");
}
