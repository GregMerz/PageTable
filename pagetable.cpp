#include <iostream>
#include <unistd.h>

struct PageTable {
    int levelCount;
    int *bitMaskAry;
    int *shiftAry;
    int *entryCount;
};

struct Level {
    void *pageTablePtr;
    int depth;
    void *nextLevelPtr;
};

struct Map {

};

unsigned int logicalToPage(unsigned int logicalAddress, unsigned int mask, unsigned int shift) {
    unsigned int ans;

    ans = logicalAddress & mask;
    ans = ans >> shift;

    return ans;
}

int main(int argc, char **argv) {
    int option;

    while ( (option = getopt(argc, argv, "n:o:")) != -1) {
        switch (option) {
        case 'n': 
            break;
        case 'o':
            break;
        default:
            // do something with this later
        }
    }


}