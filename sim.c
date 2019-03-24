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


int main(int argc, char *argv[]){
	int i;
	FILE *infp = NULL;
	int cachesize = 0, blocksize = 0;
	int associativity = 0, replacement = -1;

	if(argc != 11){
		printf("Usage: %s -f <trace file name> -s <cache size in KB> -b <block size> -a <associativity> -r <replacment policy(LRU,RR,RND)>\n", argv[0]);
		return -1;
	}

	for(i=1;i<argc;i++){
		if(strcmp(argv[i],"-f")==0){
			i++;
			infp = fopen(argv[i], "r");
			if (infp == NULL){
				printf("Unable to open %s, EXITTING\n", argv[i]);
				return -2;
			}
		}else if(strcmp(argv[i], "-s")==0){
			i++;
			cachesize = atoi(argv[i]);
			if(cachesize < 1 || cachesize > 8192){
				printf("Invalid cache size, 1KB(1) to 8MB(8192): EXITTING\n");
				return -3;
			}
		}else if(strcmp(argv[i], "-b")==0){
			i++;
			blocksize = atoi(argv[i]);
			if(blocksize < 4 || blocksize > 64){
				printf("Invalid block size, blocks are 4B->64B: EXITTING\n");
				return -4;
			}
		}else if(strcmp(argv[i], "-a") ==0){
			i++;
			associativity = atoi(argv[i]);
			if(associativity != 1 || associativity != 2 || associativity != 4 || associativity != 8 || associativity != 16){
				printf("Invalid associativity rate(1 2 4 8 16): EXITTING\n");
				return -5;
			}
		}else if(strcmp(argv[i], "-r")==0){
			i++;
			if(strcmp(argv[i], "RR")==0){
				replacement = 1; /*****replacement policy round robin == 1*******/
			}else if(strcmp(argv[i],"RND")==0){
				replacement = 3; /*****replacement policy is random == 3*********/
			}else if(strcmp(argv[i], "LRU")==0){
				replacement = 5; /******replacement is LeastRecentlyUsed == 5****/
			}else{
				printf("Invalid replacement policy(RR RND LRU): EXITING\n");
				return -6;
			}
		}else {
			printf("Invalid Command Line Argument\n");
		
		}
	}

	fclose(infp);
	return 1;
}

