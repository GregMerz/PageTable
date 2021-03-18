#include <iostream>
#include <unistd.h>
#include <cmath>
#include <stdlib.h>
#include <string.h>

struct PageTable
{
    void *rootNodePtr;
    int levelCount;
    int *bitMaskAry;
    int *shiftAry;
    int *entryCount;

    PageTable(void *m_rootNodePtr, int m_levelCount, int *m_bitMaskAry, int *m_shiftAry, int *m_entryCount)
    {
        rootNodePtr = m_rootNodePtr;
        levelCount = m_levelCount;
        bitMaskAry = m_bitMaskAry;
        shiftAry = m_shiftAry;
        entryCount = m_entryCount;
    }
};

struct Level
{
    void *pageTablePtr;
    int depth;
    void **nextLevelPtr;

    Level(void *m_pageTablePtr, int m_depth, void **m_nextLevelPtr)
    {
        pageTablePtr = m_pageTablePtr;
        depth = m_depth;
        nextLevelPtr = m_nextLevelPtr;
    }
};

struct Map
{
    bool valid;
    int frame;
};

unsigned int logicalToPage(unsigned int logicalAddress, unsigned int mask, unsigned int shift)
{
    unsigned int pageNumber;

    pageNumber = logicalAddress & mask;
    pageNumber = pageNumber >> shift;

    return pageNumber;
}

Map *pageLookup(PageTable *pageTable, unsigned int logicalAddress)
{
    Map *map;

    return map;
}

void pageInsert(PageTable *pageTable, unsigned int logicalAddress, unsigned int frame)
{
    Level *currLevel = (Level *)pageTable->rootNodePtr;

    for (int level = 0; level < pageTable->levelCount - 1; level++)
    {
        int pageNumber = logicalToPage(logicalAddress, pageTable->bitMaskAry[level], pageTable->shiftAry[level]);

        if (currLevel->nextLevelPtr[pageNumber] == NULL)
        {
            int size = pageTable->entryCount[level + 1] - 1;
            Level **levelList = new Level *[size];

            for (int idx = 0; idx < size; idx++)
            {
                levelList[idx] == NULL;
            }

            currLevel->nextLevelPtr[pageNumber] = new Level((void *)pageTable, level + 1, (void **)levelList);
        }

        currLevel = (Level *)currLevel->nextLevelPtr[pageNumber];
    }
}

int main(int argc, char **argv)
{
    // declare starting structs
    PageTable *pageTable;
    Level *firstLevel;

    // if -n flag called, says how many addresses to load
    int numberOfAddresses = -1;

    // if -o flag called, gets char*
    char *mode;

    // file where we read the addresses from
    char *traceFile;

    // when page exists hitCounter increments, otherwise missCounter increments
    int hitCounter;
    int missCounter;

    //variable for getopt
    int option;

    // arguments with flags infront
    while ((option = getopt(argc, argv, "n:o:")) != -1)
    {
        switch (option)
        {
        case 'n':
            numberOfAddresses = atoi(optarg);
            std::cout << numberOfAddresses;
            break;
        case 'o':
            mode = optarg;

            if (strcmp(mode, "bitmasks") == 0)
            {
                std::cout << "bitmasks";
            }
            if (strcmp(mode, "logical2physical") == 0)
            {
                std::cout << "logical2physical";
            }
            if (strcmp(mode, "page2frame") == 0)
            {
                std::cout << "page2frame";
            }
            if (strcmp(mode, "offset") == 0)
            {
                std::cout << "offset";
            }
            if (strcmp(mode, "summary") == 0)
            {
                std::cout << "summary";
            }

            break;
        default:
            std::cout << "This flag doesn't exist";
        }
    }

    int idx = optind;
    traceFile = argv[idx];

    int addressBits = 32;
    int levelCount = argc - (optind + 1);
    pageTable = new PageTable((void *)firstLevel, levelCount, new int[levelCount], new int[levelCount], new int[levelCount]);

    int level = 0;
    // all arguments following flagged inputs
    for (idx = optind + 1; idx < argc; idx++)
    {
        int bits = atoi(argv[idx]);
        addressBits -= bits;
        pageTable->shiftAry[level] = addressBits;
        pageTable->entryCount[level] = pow(2, bits);
        pageTable->bitMaskAry[level] = (pageTable->entryCount[level] - 1) << pageTable->shiftAry[level];

        level++;
    }

    //Level
    int levelSize = pageTable->entryCount[0] - 1;
    Level **levelList = new Level *[levelSize];

    for (int i = 0; i < levelSize; i++)
    {
        levelList[i] = NULL;
    }

    firstLevel = new Level((void *)pageTable, 0, (void **)levelList);
}