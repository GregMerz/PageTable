#include <iostream>
#include <unistd.h>
#include <cmath>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "output_mode_helpers.h"

struct PageTable
{
    void *rootNodePtr;
    int levelCount;
    unsigned int *bitMaskAry;
    int *shiftAry;
    int *entryCount;
};

struct Level
{
    void *pageTablePtr;
    int depth;
    void **nextPtr;
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
    int leafDepth = (pageTable->levelCount) - 1;
    int pageNumber;

    Level *currLevel = (Level *)pageTable->rootNodePtr;

    for (int depth = 0; depth < leafDepth; depth++)
    {
        pageNumber = logicalToPage(logicalAddress, pageTable->bitMaskAry[depth], pageTable->shiftAry[depth]);

        if (currLevel->nextPtr[pageNumber] == NULL)
        {
            return NULL;
        }

        currLevel = (Level *)currLevel->nextPtr[pageNumber];
    }

    pageNumber = logicalToPage(logicalAddress, pageTable->bitMaskAry[leafDepth], pageTable->shiftAry[leafDepth]);
    Map *map = (Map *)currLevel->nextPtr[pageNumber];

    // if the map is valid return it, otherwise return NULL
    return (map->valid) ? map : NULL;
}

Level **makeLevelList(PageTable *pageTable, int depth)
{
    int size = (pageTable->entryCount[depth]) - 1;
    Level **nextLevelPtr = new Level *[size];

    for (int idx = 0; idx < size; idx++)
    {
        nextLevelPtr[idx] == NULL;
    }

    return nextLevelPtr;
}

Map **makeMapList(PageTable *pageTable, int depth)
{
    int size = (pageTable->entryCount[depth]) - 1;
    Map **mapPtr = new Map *[size];

    for (int idx = 0; idx < size; idx++)
    {
        mapPtr[idx] = new Map(false, -1);
    }

    return mapPtr;
}

void pageInsert(Level *currLevel, unsigned int logicalAddress, unsigned int frame)
{
    PageTable *pageTable = (PageTable *)currLevel->pageTablePtr;
    int depth = currLevel->depth;
    unsigned int bitMask = pageTable->bitMaskAry[depth];
    int shift = pageTable->shiftAry[depth];

    int pageNumber = logicalToPage(logicalAddress, bitMask, shift);

    //This level is leaf Node
    int leafDepth = (pageTable->levelCount) - 1;
    if (depth == leafDepth)
    {
        // check if this is making sense later
        Map **map = (Map **)currLevel->nextPtr;
        map[pageNumber]->valid = true;
        map[pageNumber]->frame = frame;
    }
    else
    {
        void **nextPtr = (depth + 1 == leafDepth) ? (void **)makeMapList(pageTable, depth + 1) : (void **)makeLevelList(pageTable, depth + 1);

        Level *newLevel = new Level(pageTable, depth + 1, nextPtr);

        currLevel->nextPtr[pageNumber] = (void *)newLevel;
        pageInsert(newLevel, logicalAddress, frame);
    }
}

void pageInsert(PageTable *pageTable, unsigned int logicalAddress, unsigned int frame)
{
    pageInsert((Level *)(pageTable->rootNodePtr), logicalAddress, frame);
}

int main(int argc, char **argv)
{
    // declare starting structs
    PageTable pageTable;
    Level firstLevel;

    //variables for getopt
    int option;
    OutputOptionsType outputType;
    char *mode;
    int numberOfAddresses = -1;

    // arguments with flags infront
    while ((option = getopt(argc, argv, "n:o:")) != -1)
    {
        switch (option)
        {
        case 'n':
            numberOfAddresses = atoi(optarg);
            break;
        case 'o':
            mode = optarg;
            break;
        default:
            std::cout << "This flag doesn't exist";
        }
    }

    int idx = optind;
    char *traceFile = argv[idx];

    // setup for pagetable
    int addressBits = 32;
    int levelCount = argc - (optind + 1);
    pageTable.rootNodePtr = &firstLevel;
    pageTable.levelCount = levelCount;
    pageTable.bitMaskAry = new unsigned int[levelCount];
    pageTable.shiftAry = new int[levelCount];
    pageTable.entryCount = new int[levelCount];

    int level = 0;
    for (idx = optind + 1; idx < argc; idx++)
    {
        int bits = atoi(argv[idx]);
        addressBits -= bits;
        pageTable.shiftAry[level] = addressBits;
        pageTable.entryCount[level] = pow(2, bits);
        pageTable.bitMaskAry[level] = (pageTable.entryCount[level] - 1) << pageTable.shiftAry[level];

        level++;
    }

    //Level
    int levelSize = pageTable.entryCount[0] - 1;
    Level **levelList = new Level *[levelSize];

    for (int i = 0; i < levelSize; i++)
    {
        levelList[i] = NULL;
    }

    firstLevel.pageTablePtr = &pageTable;
    firstLevel.depth = 0;
    firstLevel.nextPtr = (void **)levelList;

    unsigned int offsetMask = pow(2, addressBits) - 1;

    unsigned int addresses[20] = {
        0x0041F760,
        0x0041F780,
        0x0041F740,
        0x11F5E2C0,
        0x05E78900,
        0x13270900,
        0x004758A0,
        0x004A30A0,
        0x0049E110,
        0x0049E160,
        0x0044E4F8,
        0x0044E500,
        0x0744E520,
        0x0044E5A0,
        0x388A65A0,
        0x0744EDD0,
        0x0044E5E0,
        0x0703FF10,
        0x0044E620,
        0x1D496620};

    if (strcmp(mode, "bitmasks") == 0)
    {
        report_bitmasks(pageTable.levelCount, pageTable.bitMaskAry);
    }

    if (strcmp(mode, "logical2physical") == 0)
    {
        int frame = 0;

        for (int i = 0; i < 20; i++)
        {
            Map *map;

            if ((map = pageLookup(&pageTable, addresses[i])) == NULL)
            {
                pageInsert((Level *)pageTable.rootNodePtr, addresses[i], frame);
                report_logical2physical(addresses[i], frame);
                frame++;
            }

            else
            {
                report_logical2physical(addresses[i], map->frame);
            }
        }
    }
    if (strcmp(mode, "page2frame") == 0)
    {
        for (int idx = 0; idx < 20; idx++)
        {
            unsigned int *page = new unsigned int[pageTable.levelCount];

            for (int i = 0; i < pageTable.levelCount; i++)
            {
                page[i] = logicalToPage(addresses[i], pageTable.bitMaskAry[i], pageTable.shiftAry[i]);
            }

            report_pagemap(addresses[idx], pageTable.levelCount, page, idx);
        }
    }
    if (strcmp(mode, "offset") == 0)
    {
        for (int i = 0; i < 20; i++)
        {
            report_logical2offset(addresses[i], offsetMask);
        }
    }
    if (strcmp(mode, "summary") == 0)
    {
        std::cout << "summary";
    }
}
