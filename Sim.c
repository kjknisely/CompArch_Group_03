/**********************************
Computer Architecture Group Project 
Cache Simulator
Richard Azille
Keegan Knisely
Sabrina Mosher
**********************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int ADDRESS_SPACE = 32; // 32-bit address space

void printHeader(char *argv[], int cachesize, int blocksize, int associativity, int replacement, int argc, char *infilename);
char *getPolicyString(int replacement);
void printCalculatedValues(int cachesize, int blocksize, int associativity, int replacement);
int calculateTotalBlocks(int cachesize, int blocksize);
int calculateTagSize(int blocksize, int indexSize);
int calculateIndexSize(int totalBlocks, int associativity);
int calculateTotalIndices(int indexSize);
int calculateImplementationMemorySize(int tagSize, int totalBlocks);
void printCacheHitRate();

int main(int argc, char *argv[]){
	int i;
	FILE *infp = NULL;
	int cachesize = 0, blocksize = 0;
	int associativity = 0, replacement = -1;
	char *infilename=NULL;
	if(argc != 11){
		printf("Usage: %s -f <trace file name> -s <cache size in KB> -b <block size> -a <associativity> -r <replacment policy(LRU,RR,RND)>\n", argv[0]);
		return -1;
	}

	for(i=1;i<argc;i++){
		if(strcmp(argv[i],"-f")==0){
			i++;
			infp = fopen(argv[i], "r");
			if (infp == NULL){
				printf("Unable to open %s, EXITING\n", argv[i]);
				return -2;
			}
			infilename = argv[i];
		}else if(strcmp(argv[i], "-s")==0){
			i++;
			cachesize = atoi(argv[i]);
			if(cachesize < 1 || cachesize > 8192){
				printf("Invalid cache size, 1KB(1) to 8MB(8192): EXITING\n");
				return -3;
			}
		}else if(strcmp(argv[i], "-b")==0){
			i++;
			blocksize = atoi(argv[i]);
			if(blocksize < 4 || blocksize > 64){
				printf("Invalid block size, blocks are 4B->64B: EXITING\n");
				return -4;
			}
		}else if(strcmp(argv[i], "-a")==0){
			i++;
			associativity = atoi(argv[i]);
			if(!(associativity == 1 || associativity == 2 || associativity == 4 || associativity == 8 || associativity == 16)){
				printf("Invalid associativity rate(1 2 4 8 16): EXITING\n");
				return -5;
			}
		}else if(strcmp(argv[i], "-r")==0){
			i++;
			if(strcmp(argv[i], "RR")==0){
				replacement = 1; /*****replacement policy round robin == 1*******/
			}else if(strcmp(argv[i],"RND")==0){
				replacement = 2; /*****replacement policy is random == 3*********/
			}else if(strcmp(argv[i], "LRU")==0){
				replacement = 3; /*****replacement policy is LeastRecentlyUsed == 5****/
			}else{
				printf("Invalid replacement policy(RR RND LRU): EXITING\n");
				return -6;
			}
		}else {
			printf("Invalid Command Line Argument\n");
			return -7;
		}
	}

	printHeader(argv, cachesize, blocksize, associativity, replacement, argc, infilename);
	printCalculatedValues(cachesize, blocksize, associativity, replacement);
	printCacheHitRate();

	fclose(infp);
	return 1;
}

void printHeader(char *argv[], int cachesize, int blocksize, int associativity, int replacement, int argc, char *filename){
	int i;

	printf("CS 3853 Spring 2019 - Group #3\n");
	printf("Cmd Line:");
	for(i=0;i<argc;i++)
	{		
		printf("%s ",argv[i]);
	}
	printf("\n");
	printf("Trace File: %s\n", argv[2]);
	printf("Cache Size: %d KB\n", cachesize);
	printf("Block Size: %d bytes\n", blocksize);
	printf("Associativity: %d\n", associativity);
	printf("Policy: %s\n", getPolicyString(replacement));
	printf("\n");
}

char *getPolicyString(int replacement){
	if(replacement == 1){
		return "RR";
	}else if(replacement == 2){
		return "RND";
	}else{
		return "LRU";
	}
}

void printCalculatedValues(int cachesize, int blocksize, int associativity, int replacement){
	int totalBlocks = calculateTotalBlocks(cachesize, blocksize);
	int indexSize = calculateIndexSize(totalBlocks, associativity);
	int totalIndices = calculateTotalIndices(indexSize);
	int tagSize = calculateTagSize(blocksize, indexSize);

	printf("Total # Blocks: %d K (2^%.0lf)\n", totalBlocks / 1024, log(totalBlocks/1024)/log(2)+10); 
	printf("Tag Size: %d bits\n", tagSize);
	printf("Index Size: %d bits, Total Indices: %d K\n", indexSize, totalIndices);
	printf("Implementation Memory Size: %d bytes\n", calculateImplementationMemorySize(tagSize, totalBlocks));
	printf("\n");
}

int calculateTotalBlocks(int cachesize, int blocksize){
	return cachesize * 1024 / blocksize;
}

int calculateTagSize(int blocksize, int indexSize){
	int blockBits = log(blocksize) / log(2);
	return ADDRESS_SPACE - blockBits - indexSize;
}

int calculateIndexSize(int totalBlocks, int associativity){
	return log(totalBlocks / associativity) / log(2);
}

int calculateTotalIndices(int indexSize){
	return pow(2, indexSize) / 1024;
}

int calculateImplementationMemorySize(int tagSize, int totalBlocks){
	return ((tagSize + 1) * totalBlocks) / 8;
}

void printCacheHitRate(){
	// TODO: Actually calulate and report cache hit rate for future milestones
	printf("Cache Hit Rate: __%%\n");
	printf("\n");
}
