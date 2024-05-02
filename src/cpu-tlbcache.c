/*
 * Copyright (C) 2024 pdnguyen of the HCMC University of Technology
 */
/*
 * Source Code License Grant: Authors hereby grants to Licensee 
 * a personal to use and modify the Licensed Source Code for 
 * the sole purpose of studying during attending the course CO2018.
 */
//#ifdef MM_TLB
/*
 * Memory physical based TLB Cache
 * TLB cache module tlb/tlbcache.c
 *
 * TLB cache is physically memory phy
 * supports random access 
 * and runs at high speed
 */

#define TLB_ENTRY_SIZE 8
#define TLB_SIZE(msize) (msize % TLB_ENTRY_SIZE)
typedef unsigned short HALF_WORD;
#include "mm.h"
#include <stdlib.h>

int get_tlb_index(int pid, int page_number, int msize) {
    return (pid*(TLB_SIZE(msize)+1)+ page_number)%TLB_SIZE(msize);
}

void write_to_cache(struct memphy_struct *memphy, int process_id, int page_number, int frame_number, int index) {
    TLBMEMPHY_write(memphy, index, (process_id >> 24) & 0xFF);
    TLBMEMPHY_write(memphy, index + 1, (process_id >> 16) & 0xFF);
    TLBMEMPHY_write(memphy, index + 2, (process_id >> 8) & 0xFF);
    TLBMEMPHY_write(memphy, index + 3, process_id & 0xFF);
    
    TLBMEMPHY_write(memphy, index + 4, (page_number >> 8) & 0xFF);
    TLBMEMPHY_write(memphy, index + 5, page_number & 0xFF);
    
    TLBMEMPHY_write(memphy, index + 6, (frame_number >> 8) & 0xFF);
    TLBMEMPHY_write(memphy, index + 7, frame_number & 0xFF);
}

// Function to read integers from bytes one byte at a time
void read_from_cache(struct memphy_struct* memphy, int index, int *process_id, int *page_number, int *frame_number) {
    BYTE value;
    TLBMEMPHY_read(&memphy, index, &value);
    *process_id = value << 24;
    TLBMEMPHY_read(&memphy, index + 1, &value);
    *process_id |= value << 16;
    TLBMEMPHY_read(&memphy, index + 2, &value);
    *process_id |= value << 8;
    TLBMEMPHY_read(&memphy, index + 3, &value);
    *process_id |= value;
    
    TLBMEMPHY_read(&memphy, index + 4, &value);
    *page_number = value << 8;
    TLBMEMPHY_read(&memphy, index + 5, &value);
    *page_number |= value;
    
    TLBMEMPHY_read(&memphy, index + 6, &value);
    *frame_number = value << 8;
    TLBMEMPHY_read(&memphy, index + 7, &value);
    *frame_number |= value;
}

#define init_tlbcache(mp,sz,...) init_memphy(mp, sz, (1, ##__VA_ARGS__))

/*
 *  tlb_cache_read read TLB cache device
 *  @mp: memphy struct
 *  @pid: process id
 *  @pgnum: page number
 *  @value: obtained value
 */
int tlb_cache_read(struct memphy_struct * mp, int pid, int pgnum, int* value)
{
   /* TODO: the identify info is mapped to 
    *      cache line by employing:
    *      direct mapped, associated mapping etc.
    */
    int index = get_tlb_index(pid, pgnum, mp->maxsz);
    int cached_pid, cached_pgnum, cached_frnum;
    read_from_cache(mp, index, &cached_pid,&cached_pgnum,&cached_frnum);
    if (cached_pgnum == pgnum && cached_pid == pid) {
        *value = cached_frnum;
        return 0; // TLB hit
    }
    return -1; // TLB miss
}

/*
 *  tlb_cache_write write TLB cache device
 *  @mp: memphy struct
 *  @pid: process id
 *  @pgnum: page number
 *  @value: obtained value
 */
int tlb_cache_write(struct memphy_struct *mp, int pid, int pgnum, BYTE value)
{
   /* TODO: the identify info is mapped to 
    *      cache line by employing:
    *      direct mapped, associated mapping etc.
    */
   int index = get_tlb_index(pid, pgnum, mp->maxsz);
   int cached_pid, cached_pgnum, cached_frnum;
   read_from_cache(mp, index, &cached_pid,&cached_pgnum,&cached_frnum);
    if (cached_pgnum == pgnum && cached_pid == pid) {
        TLBMEMPHY_write(mp,index,value);
        return 0; // TLB hit
    }
    return -1; // TLB miss
}

/*
 *  TLBMEMPHY_read natively supports MEMPHY device interfaces
 *  @mp: memphy struct
 *  @addr: address
 *  @value: obtained value
 */
int TLBMEMPHY_read(struct memphy_struct * mp, int addr, BYTE *value)
{
   if (mp == NULL)
     return -1;

   /* TLB cached is random access by native */
   *value = mp->storage[addr];

   return 0;
}

/*
 *  TLBMEMPHY_write natively supports MEMPHY device interfaces
 *  @mp: memphy struct
 *  @addr: address
 *  @data: written data
 */
int TLBMEMPHY_write(struct memphy_struct * mp, int addr, BYTE data)
{
   if (mp == NULL)
     return -1;

   /* TLB cached is random access by native */
   mp->storage[addr] = data;

   return 0;
}

/*
 *  TLBMEMPHY_format natively supports MEMPHY device interfaces
 *  @mp: memphy struct
 */

int TLBMEMPHY_dump(struct memphy_struct * mp)
{
   /*TODO dump memphy contnt mp->storage 
    *     for tracing the memory content
    */

   return 0;
}

/*
 *  Init TLBMEMPHY struct
 */
int init_tlbmemphy(struct memphy_struct *mp, int max_size)
{
   mp->storage = (BYTE *)malloc(max_size*sizeof(BYTE));
   mp->maxsz = max_size;

   mp->rdmflg = 1;

   return 0;
}

//#endif