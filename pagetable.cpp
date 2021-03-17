#include <iostream>
#include <unistd.h>
#include <cmath>

struct PageTable{
	Level *rootNodePtr;
    int levelCount;
    int *bitMaskAry;
    int *shiftAry;
    int *entryCount;

    PageTable(Level *m_rootNodePtr, int m_levelCount, int *m_bitMaskAry, int *m_shiftAry, int *m_entryCount) {
        rootNodePtr = m_rootNodePtr;
        levelCount = m_levelCount;
        bitMaskAry = m_bitMaskAry;
        shiftAry = m_shiftAry;
        entryCount = m_entryCount;
    }
}; 

struct Level {
    PageTable *pageTablePtr;
    int depth;
    void *nextLevelPtr;

    Level(PageTable *m_pageTablePtr, int m_depth, void *m_nextLevelPtr) {
        pageTablePtr = m_pageTablePtr;
        depth = m_depth;
        nextLevelPtr = m_nextLevelPtr;
    }
};

struct Map {
	int *virtualPage;
    int *physicalFrame;
};

unsigned int logicalToPage(unsigned int logicalAddress, unsigned int mask, unsigned int shift) {
    unsigned int pageTable;

    pageTable = logicalAddress & mask;
    pageTable = pageTable >> shift;

    return pageTable;
}

Map * pageLookup(PageTable *pageTable, unsigned int LogicalAddress) {
    Map *map;

    return map;
}

void pageInsert(PageTable *pageTable, unsigned int LogicalAddress, unsigned int frame) {

}

int main(int argc, char **argv) {
    int option;
    int idx;
    int numberOfAddresses;
    char *mode;
    char *traceFile;
    int hitCounter;
    int missCounter;
		
    // arguments with flags infront
    while ( (option = getopt(argc, argv, "n:o:")) != -1) {
        switch (option) {
        case 'n': 
        		numberOfAddresses = atoi(optarg);
            break;
        case 'o':
        /*
        	*mode = optarg;
            if ()
            if ()
            if ()
            if ()
            if ()
        */
            break;
        default:
            // do something with this later
        }
    }

	idx = optind;
    traceFile = argv[idx];
 		
    int addressBits = 32;
    int levelCount = argc - (optind + 1);
    PageTable pageTable(NULL, levelCount, new int[levelCount], new int[levelCount], new int[levelCount]);
    
    int level = 0;
	// all arguments following flagged inputs
	for (idx = optind + 1; idx < argc; idx++) {
    	int bits = atoi(argv[idx]);
        addressBits -= bits;
        pageTable.shiftAry[level] = addressBits;
        pageTable.entryCount[level] = pow(2, bits);
        pageTable.bitMaskAry[level] = (pageTable.entryCount[level] - 1) << pageTable.shiftAry[level];
        
        level++;
    }
    
    //Level
    Level firstLevel(&pageTable, 0, new Level*[pageTable.entryCount[0] - 1]);
    pageTable.rootNodePtr = &firstLevel;
}