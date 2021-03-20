#ifndef OUTPUT_MODE_HELPERS_H
#define OUTPUT_MODE_HELPERS_H

/*
 * Compilation notes:
 * C compilers:
 *    uses bool, must compile with -std=c99 or later (bool was introduced
 *    in the 1999 C standard.
 *
 * C++ compilers
 *    uses uint32_t, unsigned 32 bit integer type, introduced in C++11,
 *    The defaults in the g++ compiler on edoras should be fine with this
 */

/* C and C++ define some of their types in different places.
 * Check and see if we are using C or C++ and include appropriately
 * so that this will compile under C and C++
 */
#ifdef __cplusplus
/* C++ includes */
#include <stdint.h>
#else
/* C includes */
#include <inttypes.h>
#include <stdbool.h>
#endif 

/*
 * structure that can be used to maintain which output types are enabled.
 * Note that this uses the bool keyword. 
 *
 * If compiled with a C compiler, make sure that the C99 dialect or later is used.
 * (-std=c99 with a GNU C compiler)
 */
typedef struct {
  bool bitmasks;  /* display bitmaks */
  bool logical2physical;  /* show logical to physical address translation */
  bool page2frame;  /* show mapping between page numbers and frame */
  bool offset; /* show the offset of each address */
  bool summary; /* summary statistics */
} OutputOptionsType;

/*
 * report_logical2physical
 * Given a pair of numbers, output a line: 
 *      src -> dest  
 */
void report_logical2physical(uint32_t src, uint32_t dest);

/*
 * report_logical2offselt
 * Given a logical address and its offset, output a line:
 *	logical -> offset
 */
void report_logical2offset(uint32_t logical, uint32_t offset);

/*
 * report_summary
 * Write out a mesasge that indicates summary information for the page table.
 * page_size - Number of bytes per page
 * hits - Number of times a page was mapped.
 * frames_used - Number of frames allocated
 * addreses - Number of addresses processed
 * bytes - Total number of bytes needed for data structure.  
 *	   Should include all levels, allocated arrays, etc.
 */
void report_summary(unsigned int page_size, unsigned int hits, 
		    unsigned int addresses, unsigned int frames_used,
		    unsigned int bytes);

/*
 * report_bitmasks
 * Write out bitmasks.
 * levels - Number of levels
 * masks - Pointer to array of bitmasks
 */
void report_bitmasks(int levels, uint32_t *masks);

/*
 * report_pagemap
 * Write out page numbers and frame number
 *
 * logical_addr - logical address, displayed first (primarily to help
 *                with debugging)
 * levels - specified number of levels in page table
 * pages - pages[idx] is the page number associated with level idx
 *         (0 < idx < levels)
 * frame - page is mapped to specified frame
 */
void report_pagemap(uint32_t logical_addr, int levels, 
		    uint32_t *pages, uint32_t frame);

#endif
