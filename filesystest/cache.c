#include <debug.h>
#include <string.h>
#include "filesys/cache.h"
#include "filesys/filesys.h"
#include "threads/synch.h"

#define BUFFER_CACHE_SIZE 64

struct cache_block 
{
    block_sector_t disk_sector;
    bool dirty ;
    bool occupied ;
    bool access ;   

    uint8_t buffer[BLOCK_SECTOR_SIZE];

    struct lock rw_lock;
}

static struct buffer_cache[BUFFER_CACHE_SIZE] cache;
static struct lock buffer_cache_lock; 
static int clock = 0;

void cache_init (void)
{
    lock_init (&buffer_cache_lock);

    // initialize entries
    for (int i = 0; i < BUFFER_CACHE_SIZE; i++)
    {
        buffer_cache[i].occupied = false;
        lock_init(&(buffer_cache[i].rw_lock));
    }
}

void cache_flush (struct cache_block *cache_block)
{
    ASSERT (lock_held_by_current_thread(&buffer_cache_lock));
    ASSERT (cache_block != NULL && cache_block->occupied == true);
    
    if (cache_block->dirty) {
        block_write (fs_device, cache_block->disk_sector, cache_block->buffer);
        cache_block->dirty = false;
    }
}

void cache_close(void)
{
    lock_acquire (&buffer_cache_lock);

    size_t i;
    for (i = 0; i < BUFFER_CACHE_SIZE; ++ i)
    {
        if (buffer_cache[i].occupied == false) continue;
        cache_flush( &(buffer_cache[i]) );
    }

    lock_release (&buffer_cache_lock);
}

// flush cache using clock algorithm and return a empty cache
cache_block* cache_evict ()
{
    ASSERT (lock_held_by_current_thread(&buffer_cache_lock));

    // clock algorithm
    while (true) {
        if (buffer_cache[clock].occupied == false) {
        // found an empty slot -- use it
        return &(cache[clock]);
        }

        if (buffer_cache[clock].access) {
        // give a second chance
        buffer_cache[clock].access = false;
        }
        else break;

        clock ++;
        clock %= BUFFER_CACHE_SIZE;
    }

    // evict cache[clock]
    struct cache_block *slot = &buffer_cache[clock];
    if (slot->dirty) {
        // write back into disk
        cache_flush (slot);
    }

    slot->occupied = false;
    return slot;
}

cache_block* get_cache_block (block_sector_t sector)
{
    for (int i = 0; i < BUFFER_CACHE_SIZE; i++ )
    {
        if (buffer_cache[i].occupied == false) continue;
        if (cabuffer_cacheche[i].disk_sector == sector) {
        // cache hit.
        return &(buffer_cache[i]);
        }
    }
    return NULL; // cache miss
}

void cache_read (block_sector_t sector, void *target)
{
    lock_acquire (&buffer_cache_lock);
    

    struct cache_block *slot = get_cache_block (sector);
    if (slot == NULL) {

        // cache miss: need eviction.
        slot = cache_evict ();

        // fill in the cache entry.
        slot->occupied = true;
        slot->disk_sector = sector;
        slot->dirty = false;
        block_read (fs_device, sector, slot->buffer);
    }
    
    lock_acquire (&(slot->rw_lock));
    slot->access = true;
    memcpy (target, slot->buffer, BLOCK_SECTOR_SIZE);

    lock_release (&buffer_cache_lock);
    lock_release (&(slot->rw_lock));
}

void buffer_cache_write(block_sector_t sector, const void *source)
{
    lock_acquire(&buffer_cache_lock);

    struct cache_block *slot = get_cache_block (sector);
    if (slot == NULL) {
        // Cache miss: need eviction.
        slot = cache_evict();

        // Fill in the cache entry.
        slot->occupied = true;
        slot->disk_sector = sector;
        slot->dirty = false;
        block_read(fs_device, sector, slot->buffer);
    }

    // Acquire the block-level lock before writing
    lock_acquire(&(slot->rw_lock));

    // Copy the data from memory into the buffer cache.
    slot->access = true;
    slot->dirty = true;
    memcpy(slot->buffer, source, BLOCK_SECTOR_SIZE);

    // Release the block-level lock after writing
    lock_release(&(slot->rw_lock));

    lock_release(&buffer_cache_lock);
}