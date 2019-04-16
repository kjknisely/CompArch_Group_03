/*** CACHE AND COMPONENT STRUCTURES ***/

typedef struct {
        int valid;
        unsigned int tag;
} Block;

typedef struct {
        int replacementTag;
        Block *blocks;
} Index;

typedef struct {
        int cachesize;
        int blocksize;
        int associativity;
        int replacement;
        int totalBlocks;
	int indexSize;
	int totalIndices;
	int tagSize;
	int offsetSize;
	Index *indices;
} Cache;

/*** FUNCTION PROTOTYPES ***/

void printHeader(char *argv[], int argc, char *infilename);
void printCalculatedValues();
void printCacheHitRate();
void printTraceData(int eAddr, int eSize, int wAddr, int rAddr);
void parseTrace(FILE * traceFile);
void parseDataLine(char *buf, int *writeAddr, int *readAddr);
void parseEipLine(char * buf, int *addr, int *size);
int calculateTotalBlocks();
int calculateTagSize(int indexSize);
int calculateIndexSize(int totalBlocks);
int calculateTotalIndices(int indexSize);
int calculateOverheadMemorySize(int tagSize, int totalBlocks);
void configureCache(); 
void printCache();
void freeCache();
char *getPolicyString(int replacement);
