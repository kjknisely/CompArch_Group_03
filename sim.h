/*** CACHE AND COMPONENT STRUCTURES ***/

/*
 * Block
 */
typedef struct {
    long clockTm;
    int valid;
    unsigned int tag;
} Block;

/*
 * Index 
 */
typedef struct {
    int replacementBlockIndex;
    Block *blocks;
} Index;

/*
 * Cache structure
 */
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
    double compulsoryMisses;
	double accessCount;
} Cache;

/*** FUNCTION PROTOTYPES ***/
void printHeader(char *argv[], int argc, char *infilename);
void printCalculatedValues();
void printCacheHitRate();
void parseTrace(FILE * traceFile);
void parseDataLine(char *buf, int *writeAddr, int *readAddr);
void parseEipLine(char * buf, int *addr, int *size);
int calculateTotalBlocks();
int calculateTagSize();
int calculateIndexSize();
int calculateTotalIndices();
int calculateOverheadMemorySize(int tagSize, int totalBlocks);
void configureCache(); 
void printCache();
void freeCache();
char *getPolicyString(int replacement);
void performCacheOperation(int addr, int size);
void performCacheAccess(unsigned int tag, unsigned int index);
int isHit(unsigned int index, unsigned int tag);
void replace(unsigned int index, unsigned int tag);
void replaceRR(unsigned int index, unsigned int tag);
void replaceRandom(unsigned int index, unsigned int tag);
void replaceLRU(unsigned int index, unsigned int tag);
