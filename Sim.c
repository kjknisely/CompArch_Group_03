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
#include <time.h>
#include "sim.h"

static int ADDRESS_SPACE = 32; // 32-bit address space

Cache cache;
long CLOCK = 0;

int main(int argc, char *argv[]){
    srand(time(NULL));
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
        // cache size
        }else if(strcmp(argv[i], "-s")==0){
            i++;
            cache.cachesize = atoi(argv[i]);
            if(cache.cachesize < 1 || cache.cachesize > 8192){
                printf("Invalid cache size, 1KB(1) to 8MB(8192): EXITING\n");
                return -3;
            }
        // block size
        }else if(strcmp(argv[i], "-b")==0){
            i++;
            cache.blocksize = atoi(argv[i]);
            if(cache.blocksize < 4 || cache.blocksize > 64){
                printf("Invalid block size, blocks are 4B->64B: EXITING\n");
                return -4;
            }
        // associativity
        }else if(strcmp(argv[i], "-a")==0){
            i++;
            cache.associativity = atoi(argv[i]);
            if(!(cache.associativity == 1 || cache.associativity == 2 || cache.associativity == 4 || cache.associativity == 8 || cache.associativity == 16)){
                printf("Invalid associativity rate(1 2 4 8 16): EXITING\n");
                return -5;
            }
        // replacement policy
        }else if(strcmp(argv[i], "-r")==0){
            i++;
            if(strcmp(argv[i], "RR")==0){
                cache.replacement = 1;
            }else if(strcmp(argv[i],"RND")==0){
                cache.replacement = 2;
            }else if(strcmp(argv[i], "LRU")==0){
                cache.replacement = 3; 
            }else{
                printf("Invalid replacement policy(RR, RND, LRU): EXITING\n");
                return -6;
            }
        }else {
            printf("Invalid Command Line Argument\n");
            return -7;
        }
    }

    configureCache();
    parseTrace(infp);
    // printCache();
    printHeader(argv, argc, infilename);
    printCalculatedValues();
    printCacheHitRate();
    freeCache();

    fclose(infp);
    return 0;
}

/*
 * Configure cache structure based on total indices and associativity
 */
void configureCache()
{
    // Calculate additional cache values
    cache.compulsoryMisses = 0;
    cache.missCount = 0;
    cache.accessCount = 0;
    cache.totalBlocks = calculateTotalBlocks();
    cache.indexSize = calculateIndexSize();
    cache.totalIndices = calculateTotalIndices();
    cache.tagSize = calculateTagSize();
    cache.offsetSize = (log(cache.blocksize)/(log(2)));
    
    // Allocate cache memory
    cache.indices = calloc(cache.totalIndices, sizeof(Index));
    int i;
    for (i = 0; i < cache.totalIndices; i++) {
        cache.indices[i].blocks = calloc(cache.associativity, sizeof(Block));
        cache.indices[i].replacementBlockIndex = 0;

        // set each block to invalid
        int j;
        for (j = 0; j < cache.associativity; j++) {
                cache.indices[i].blocks[j].valid = 0;
        }
    }
}

/*
 * Displays cache structure using print statements (not necessary, but can be useful in debugging)
 */ 
void printCache()
{
    printf("\n***TOP OF CACHE***\n\n");

    int i, j;
    for (i = 0; i < cache.totalIndices; i++) {
        for (j = 0; j < cache.associativity; j++) {
            printf("%d ", cache.indices[i].blocks[j].valid);
            if (cache.indices[i].blocks[j].valid == 1) {
                printf("%-8x ", cache.indices[i].blocks[j].tag);
            }
            else {
                printf("%-8s ", "--------");
            }
        }
        printf("\n");
    }
    printf("\n***BOTTOM OF CACHE***\n\n");
}

/*
 * Free all malloc'd cache memory
 */ 
void freeCache()
{
    int i;
    for (i = 0; i < cache.totalIndices ; i++) {
        free(cache.indices[i].blocks);
    }
    free(cache.indices);
}

/*
 * Parse the input trace file
 */
void parseTrace(FILE * traceFile) {
    char buf[1000];
    int eAddr, eSize, wAddr, rAddr;

    // read to eof
      while (fgets(buf, 999, traceFile) != NULL)
      {
        // not EIP line
        if (buf[0] == 'E') {
            parseEipLine(buf, &eAddr, &eSize);
            performCacheOperation(eAddr, eSize);
        }
          else if (buf[0] == 'd') {
              parseDataLine(buf, &wAddr, &rAddr);
            if (wAddr != 0){
                performCacheOperation(wAddr, 4);
            }
            if (rAddr !=0){
                performCacheOperation(rAddr, 4);
            }
        }
    }
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
 * Splits a given address into its separate components and calls helper function to access the cache
 * based on tag and index values.
 */
void performCacheOperation(int addr, int size) {
    unsigned int tag = addr >> (cache.indexSize + cache.offsetSize);
    unsigned int index = addr << cache.tagSize; 
    index = index >> (cache.tagSize + cache.offsetSize);
    unsigned int offset = addr << (cache.tagSize + cache.indexSize);
    offset = offset >> (cache.tagSize + cache.indexSize);

    performCacheAccess(tag, index);


    if (offset + size <= cache.blocksize)
        return;

    addr += cache.blocksize;

    // printf("(%x):\tTag: %x\tIndex: %x\tOffset: %x\n", addr, tag, index, offset);
    performCacheOperation(addr, size-cache.blocksize);
}

/*
 * Accesses the access and determines whether a hit or miss occurs and whether or not a replacement
 * is necessary. Updates missCount and accessCount accordingly.
 */
void performCacheAccess(unsigned int tag, unsigned int index){
    CLOCK++;
    cache.accessCount++;
    if (isHit(index, tag) == 0) {
        cache.missCount++;

        // Place tag in first empty block if any exist
        int i;
        for (i = 0; i < cache.associativity; i++) {
            if (cache.indices[index].blocks[i].valid == 0) {

                // update LRU if replacement is LRU
                if (cache.replacement == 3)
                    cache.indices[index].blocks[i].clockTm = CLOCK;

                // update block to valid and uddate block tag
                cache.indices[index].blocks[i].tag = tag;
                cache.indices[index].blocks[i].valid = 1;
                cache.compulsoryMisses++;
                return;
            }
        }
        // No empty blocks exist, replace based on replacement policy
        replace(index, tag); // set nextReplacementBlock tag to tag
    }

    return;
}

/*
 * Determines whether or not a hit occurred
 */
int isHit(unsigned int index, unsigned int tag) {
    int i;
    for (i = 0; i < cache.associativity; i++) {
        if (cache.indices[index].blocks[i].valid == 1
            && cache.indices[index].blocks[i].tag == tag) {

            // update LRU if replacement is LRU
            if (cache.replacement == 3)
                cache.indices[index].blocks[i].clockTm = CLOCK;
            return 1;
        }
    }
    return 0;
}

/*
 * Determine which replacement function to call based on the cache's replacement policy
 */
void replace(unsigned int index, unsigned int tag) {
    if (cache.replacement == 1) {
        replaceRR(index, tag);
    } else if (cache.replacement == 2) {
        replaceRandom(index, tag);
    } else {
        replaceLRU(index, tag);
    }
}

/*
 * Replace block @ replacementBlockIndex and update replacementBlockIndex to next index sequentially
 */
void replaceRR(unsigned int index, unsigned int tag) {
    int replacementBlockIndex = cache.indices[index].replacementBlockIndex;
    cache.indices[index].blocks[replacementBlockIndex].tag = tag;
    cache.indices[index].blocks[replacementBlockIndex].valid = 1;
    cache.indices[index].replacementBlockIndex = (cache.indices[index].replacementBlockIndex + 1) % cache.associativity;
}

/*
 * Replace block @ random index between 0 and cache.associativity
 */
void replaceRandom(unsigned int index, unsigned int tag) {
    int i = rand() % cache.associativity;
    int replacementBlockIndex = i; // rand() % cache.associativity;
    cache.indices[index].blocks[replacementBlockIndex].tag = tag;
    cache.indices[index].blocks[replacementBlockIndex].valid = 1;
}

void replaceLRU(unsigned int index, unsigned int tag) {
    int i;
    Block* minBlock = NULL;

    // find the least recently used block
    for (i = 0; i < cache.associativity; i++)
        if (minBlock == NULL || cache.indices[index].blocks[i].clockTm < minBlock->clockTm)
            minBlock = &cache.indices[index].blocks[i];
    
    // set params
    minBlock->clockTm = CLOCK;
    minBlock->tag = tag;
    minBlock->valid = 1;
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
    printf("Cache Size: %d KB\n", cache.cachesize);
    printf("Block Size: %d bytes\n", cache.blocksize);
    printf("Associativity: %d\n", cache.associativity);
    printf("Policy: %s\n", getPolicyString(cache.replacement));
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
void printCalculatedValues() {
    if (cache.totalBlocks < 1024)
    {
        printf("Total # Blocks: %d (2^%.0lf)\n", cache.totalBlocks, (log(cache.totalBlocks) / (log(2))));
    }
    else
    {
        printf("Total # Blocks: %d KB (2^%.0lf)\n", cache.totalBlocks / 1024, log(cache.totalBlocks / 1024) / log(2) + 10);
    }
    printf("Tag Size: %d bits\n", cache.tagSize);
    if (cache.indexSize < 10) {
        printf("Index Size: %d bits, Total Indices: %d\n", cache.indexSize, cache.totalIndices);
    }
    else {
        printf("Index Size: %d bits, Total Indices: %d K\n", cache.indexSize, cache.totalIndices / 1024);
    }
    printf("Overhead Memory Size: %d bytes\n", calculateOverheadMemorySize(cache.tagSize, cache.totalBlocks));
    printf("Implementation Memory Size: %d bytes\n", cache.cachesize * 1024 + calculateOverheadMemorySize(cache.tagSize, cache.totalBlocks));
    printf("\n");
}

/*
 * Calculate the total number of blocks in the cache
 */
int calculateTotalBlocks(){
    return cache.cachesize * 1024 / cache.blocksize;
}

/* 
 * Calculate the size of the tag
 */
int calculateTagSize(){
    int blockBits = log(cache.blocksize) / log(2);
    return ADDRESS_SPACE - blockBits - cache.indexSize;
}

/* 
 * Calculate the size of the index
 */
int calculateIndexSize(){
    return log(cache.totalBlocks / cache.associativity) / log(2);
}

/* 
 * Calculate the total number of indices
 */
int calculateTotalIndices(){
    return pow(2, cache.indexSize); // / 1024;
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
    if (cache.accessCount == 0) {
        perror("Unable to calculate hit rate: Access count is 0.");
        return;
    }
    printf("Total Cache Accesses: %.0f\n", cache.accessCount);
    printf("Cache Hits: %.0f\n", cache.accessCount - cache.missCount);
    printf("Cache Misses: %.0f\n", cache.missCount);
    printf("Compulsory Cache Misses: %.0f\n", cache.compulsoryMisses);
    printf("Conflict Misses: %.0f\n", cache.missCount - cache.compulsoryMisses);
    printf("Cache Miss Rate: %.4f %%\n\n", 100 * cache.missCount/cache.accessCount);
}
