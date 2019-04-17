/*** CACHE AND COMPONENT STRUCTURES ***/

typedef struct {
        int valid;
        unsigned int tag;
} Block;

typedef struct {
        int replacementBlockIndex;
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
	double missCount;
	double accessCount;
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
void performCacheOperation(int addr);
void performCacheAccess(unsigned int tag, unsigned int index);
int isHit(unsigned int index, unsigned int tag);
void replace(unsigned int index, unsigned int tag);
void replaceRR(unsigned int index, unsigned int tag);
void replaceRandom(unsigned int index, unsigned int tag);
void replaceLRU(unsigned int index, unsigned int tag);
