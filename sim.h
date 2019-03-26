void printHeader(char *argv[], int argc, char *infilename);
char *getPolicyString(int replacement);
void printCalculatedValues();
int calculateTotalBlocks(int cachesize, int blocksize);
int calculateTagSize(int blocksize, int indexSize);
int calculateIndexSize(int totalBlocks, int associativity);
int calculateTotalIndices(int indexSize);
int calculateImplementationMemorySize(int tagSize, int totalBlocks);
void printCacheHitRate();
void parseTrace(FILE * traceFile);
void printTraceData(int eAddr, int eSize, int wAddr, int rAddr);
void parseDataLine(char *buf, int *writeAddr, int *readAddr);
void parseEipLine(char * buf, int *addr, int *size);