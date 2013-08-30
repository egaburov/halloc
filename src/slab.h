#ifndef HALLOC_SLAB_H_
#define HALLOC_SLAB_H_

/** @file slab.h slab (superblock) header file */

#include "utils.h"

/** possible slab states */
enum {
	SB_FREE = 0,
	SB_HEAD, SB_ROOMY, SB_BUSY
};

/** possible slab flags */
enum {
	/** slab allocated from CUDA device-side memory, and must be freed into it */
	SB_CUDA = 0x1
};

/** superblock descriptor type; information is mostly changing; note that during
		allocation, a superblock is mostly identified by superblock id */
typedef struct {
	/** slab size id 
			TODO: check if we really need it
	 */
	uint size_id;
	/** slab chunk size */
	uint chunk_sz;		
	/** slab flags */
	//uint flags;
	/** slab chunk id (currently ignored) */
	//uint chunk_id;
	/** slab state */
	//uint state;
	/** slab mutex (for changing states) */
	//uint mutex;
	/** pointer to memory owned by superblock */
	void *ptr;
} superblock_t;

/** a step to check whether the slab can be moved to another free category */
#define SB_FREE_STEP 2048
/** maximum number of tries inside a superblock after which the allocation
		attempt is abandoned */
#define MAX_NTRIES 256
/** the number of steps after which count check needs be peformed, to ensure
		that the allocator is not searching in a block that is already full */
#define CHECK_NTRIES 8
/** a "no-sb" constant */
#define SB_NONE (~0)
/** number of heads between which to distribute allocations */
#define NHEADS 1
/** whether to cache head slabs */
#define CACHE_HEAD_SBS 1

/** positions and sizes related to slab counters */
#define SB_SIZE_POS 0
#define SB_SIZE_SZ 6
#define SB_CHUNK_POS 6
#define SB_CHUNK_SZ 3
#define SB_HEAD_POS 9
#define SB_HEAD_SZ 1
#define SB_COUNT_POS 10
#define SB_COUNT_SZ 22

// functions for manipulation with counter values
/** gets slab allocation count */
__device__ inline uint sb_count(uint counter) {
	return counter >> SB_COUNT_POS;
}
/** gets size id  */
__device__ inline uint sb_size_id(uint counter) {
	return (counter >> SB_SIZE_POS) & ((1 << SB_SIZE_SZ) - 1);
}
/** gets chunk id */
__device__ inline uint sb_chunk_id(uint counter) {
	return (counter >> SB_CHUNK_POS) & ((1 << SB_CHUNK_SZ) - 1);
}
/** gets whether the slab is head (i.e., head bit is set) */
__device__ inline bool sb_is_head(uint counter) {
	return (counter >> SB_HEAD_POS) & 1;
}
/** gets the counter value for the specified count, size id and chunk id */
__host__ __device__ inline uint sb_counter_val
(uint count, bool is_head, uint chunk_id, uint size_id) {
	return count << SB_COUNT_POS | (is_head ? 1 : 0) << SB_HEAD_POS |
		(chunk_id & ((1 << SB_CHUNK_SZ) - 1)) << SB_CHUNK_POS | 
		(size_id & ((1 << SB_SIZE_SZ) - 1)) << SB_SIZE_POS;
}
/** atomically increments/decrements slab counter, returns old slab counter value */
__device__ inline uint sb_counter_inc(uint *counter, uint change) {
	return atomicAdd(counter, change << SB_COUNT_POS);
}
__device__ inline uint sb_counter_dec(uint *counter, uint change) {
	return atomicSub(counter, change << SB_COUNT_POS);
}

/** a single-thread-in-warp slab lock; it loops until the slab is locked */
// __device__ inline void sb_lock(superblock_t *sb) {
// 	lock(&sb->mutex);
// }
/** a single-thread-in-warp slab unlock; it loops until the slab is unlocked */
// __device__ inline void sb_unlock(superblock_t *sb) {
// 	unlock(&sb->mutex);
// }

#endif
