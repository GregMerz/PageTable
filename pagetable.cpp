// makefile, verify bytes implemented, test code in gradescope, output file, organize methods, comment, pair affidavid

#include <iostream>
#include <unistd.h>
#include <cmath>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fstream>
#include <vector>
#include "output_mode_helpers.h"
#include "byutr.h"

#define OOPS 5

using namespace std;

struct PageTable
{
    void *rootNodePtr;
    int levelCount;
    unsigned int *bitMaskAry;
    int *shiftAry;
    int *entryCount;
    unsigned int bytesImplemented;
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

    Map(bool m_valid, int m_frame)
    {
        valid = m_valid;
        frame = m_frame;
    }
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
    int size = pageTable->entryCount[depth];
    Level **nextLevelPtr = new Level *[size];
    pageTable->bytesImplemented += sizeof(Level **);

    for (int idx = 0; idx < size; idx++)
    {
        nextLevelPtr[idx] = NULL;
    }

    return nextLevelPtr;
}

Map **makeMapList(PageTable *pageTable, int depth)
{
    int size = pageTable->entryCount[depth];
    Map **mapPtr = new Map *[size];
    pageTable->bytesImplemented += sizeof(Map **) + (size * sizeof(Map *));

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
        Level *newLevel = new Level;
        newLevel->pageTablePtr = pageTable;
        newLevel->depth = depth + 1;
        pageTable->bytesImplemented += sizeof(Level *);

        if (currLevel->nextPtr[pageNumber] == NULL)
        {
            newLevel->nextPtr = (depth + 1 == leafDepth) ? (void **)makeMapList(pageTable, depth + 1) : (void **)makeLevelList(pageTable, depth + 1);
        }

        else
        {
            newLevel->nextPtr = ((Level *)(currLevel->nextPtr[pageNumber]))->nextPtr;
        }

        currLevel->nextPtr[pageNumber] = newLevel;
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
    //  OutputOptionsType outputType;
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
            break;
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
    pageTable.bytesImplemented = sizeof(PageTable);

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
    void **levelList = (pageTable.levelCount - 1 == 0) ? (void **)makeMapList(&pageTable, 0) : (void **)makeLevelList(&pageTable, 0);

    firstLevel.pageTablePtr = &pageTable;
    firstLevel.depth = 0;
    firstLevel.nextPtr = (void **)levelList;
    pageTable.bytesImplemented += sizeof(Level);

    unsigned int offsetMask = pow(2, addressBits) - 1;
    int frameShift = addressBits;

    vector<int> addresses;

    FILE *fp = fopen("trace.sample.tr", "r");
    int frame = 0;
    int hits = 0;

    if (fp == NULL)
    {
        printf("uh oh\n");
        exit(OOPS);
    }

    /* Start reading addresses */
    p2AddrTr trace_item; /* Structure with trace information */
    bool done = false;

    if (numberOfAddresses == -1)
    {
        while (!done)
        {
            // Grabe the next address
            int bytesread = NextAddress(fp, &trace_item);
            // Check if we actually got something
            done = bytesread == 0;
            if (!done)
            {
                addresses.push_back(trace_item.addr);
                //printf("Address %x, AddressesIdx: %i\n", trace_item.addr, addressesRead);
                if (pageLookup(&pageTable, trace_item.addr) == NULL)
                {
                    pageInsert(&pageTable, trace_item.addr, frame);
                    frame++;
                }
                else
                {
                    hits++;
                }
            }
        }
    }

    else
    {
        int addressesRead = 0;

        while (addressesRead < numberOfAddresses && !done)
        {
            // Grabe the next address
            int bytesread = NextAddress(fp, &trace_item);
            // Check if we actually got something
            done = bytesread == 0;
            if (!done)
            {
                addressesRead++;
                addresses.push_back(trace_item.addr);
                //printf("Address %x, AddressesIdx: %i\n", trace_item.addr, addressesRead);
                if (pageLookup(&pageTable, trace_item.addr) == NULL)
                {
                    pageInsert(&pageTable, trace_item.addr, frame);
                    frame++;
                }
                else
                {
                    hits++;
                }
            }
        }
    }

    // done
    if (strcmp(mode, "bitmasks") == 0)
    {
        report_bitmasks(pageTable.levelCount, pageTable.bitMaskAry);
    }

    // done
    if (strcmp(mode, "logical2physical") == 0)
    {
        for (int i = 0; i < addresses.size(); i++)
        {
            frame = pageLookup(&pageTable, addresses.at(i))->frame;
            int physicalAddress = (frame << frameShift) | (addresses.at(i) & offsetMask);
            report_logical2physical(addresses.at(i), physicalAddress);
        }
    }

    // done
    if (strcmp(mode, "page2frame") == 0)
    {
        unsigned int page[pageTable.levelCount];

        for (int idx = 0; idx < addresses.size(); idx++)
        {
            for (int i = 0; i < pageTable.levelCount; i++)
            {
                page[i] = logicalToPage(addresses.at(idx), pageTable.bitMaskAry[i], pageTable.shiftAry[i]);
            }

            frame = pageLookup(&pageTable, addresses.at(idx))->frame;

            report_pagemap(addresses.at(idx), pageTable.levelCount, page, frame);
        }
    }

    // done
    if (strcmp(mode, "offset") == 0)
    {
        for (int i = 0; i < addresses.size(); i++)
        {
            int offset = offsetMask & addresses.at(i);
            report_logical2offset(addresses.at(i), offset);
        }
    }

    // have to do later
    if (strcmp(mode, "summary") == 0)
    {
        //bytes = 8,388,680
        report_summary(trace_item.size, hits, addresses.size(), frame, pageTable.bytesImplemented);
    }
}