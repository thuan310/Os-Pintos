#ifndef FILESYS_CACHE_H
#define FILESYS_CACHE_H

#include "devices/block.h"


void cache_init ();
void cache_read (block_sector_t sector, void *target);
void cache_write (block_sector_t sector, void *source);

// flush all dirtied cache block
void cache_close ()


#endif