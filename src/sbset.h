#ifndef HALLOC_SBSET_H_
#define HALLOC_SBSET_H_

/** @file sbset.h slab set definitions */

#include "utils.h"

/** superblock set type; word 0 is actually an additional counter */
typedef uint sbset_t[SB_SET_SZ];
//typedef uint *sbset_t;

//#define WORD_SZ2 64

/** gets superblock from set (and removes it) */
__device__ inline uint sbset_get_from(sbset_t sbset);

/** adds ("returns") superblock to the set */
__device__ inline void sbset_add_to(sbset_t sbset, uint sb) {
	uint iword = sb / WORD_SZ, ibit = sb % WORD_SZ;
	uint mask = 1 << ibit;
	//atomicAdd((int *)&sbset[SB_SET_SZ - 1], 1);
	if(!(atomicOr(&sbset[iword], mask) & mask))
	 	atomicAdd((int *)&sbset[SB_SET_SZ - 1], 1);
	//atomicAdd((int *)&sbset[SB_SET_SZ - 1], 
	//					1 - ((atomicOr(&sbset[iword], mask) & mask) >> ibit));
}  // sbset_add_to

/** removes the specified slab from set */
__device__ inline void sbset_remove_from(sbset_t sbset, uint sb) {
	uint iword = sb / WORD_SZ, ibit = sb % WORD_SZ;
	uint mask = 1 << ibit;
	if(atomicAnd(&sbset[iword], ~mask) & mask)
		atomicSub((int *)&sbset[SB_SET_SZ - 1], 1);
}  // sbset_remove_from

#endif
