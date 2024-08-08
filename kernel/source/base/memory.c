#include "memory.h"
#include "rlock.h"
#include "info.h"
#include "math.h"

static uint32_t head;
static void* lock;

#define mrlock() resource_lock_request(&lock, (void*)(info_get_processor_id()+1))
#define mflock() resource_lock_free(&lock, (void*)(info_get_processor_id()+1))

void memory_init()
{
	head = MemoryBeginAddress;
	uint32_t* headpntr = (uint32_t*) head;
	*headpntr = MemoryEnd;
	*(headpntr+1) = MemorySize;
}

static char* malloc(uint32_t size)
{
	if(head == 0) return NULL;

	// make size a multiple of 8
	size += (8 - size % 8)%8;

	if(size == 0)
		return NULL;

	// head points to the end of memory
	if(head == MemoryEnd)
		return NULL;

	// the pointer to the block before
	uint32_t* block_before = &head;
	// the pointer to the current block
	uint32_t* current_block = (uint32_t*) head;
	// the size of current block
	uint32_t current_size = *(current_block+1);

	// loop untill first fit or end
	while(current_size < size && *current_block != MemoryEnd){
		block_before = current_block;
		current_block = (uint32_t*)(*current_block);
		current_size = *(current_block+1);
	}

	// size found equals the size desired; remove current block
	if(current_size == size)
	{
		*block_before = *current_block;
		return (char*) current_block;
	}
	// size found greater than size desired; create new block with the size remained
	else if(current_size > size)
	{
		uint32_t* new_block = (uint32_t*)((uint32_t) current_block + size);
		*new_block = *current_block;
		*(new_block+1) = current_size - size;
		*block_before = (uint32_t) new_block;
		return (char*) current_block;
	}
	// no size fit found
	return NULL;
}

char* alloc(uint32_t size){
	mrlock();
	char* ofs = malloc(size);
	mflock();
	return ofs;
}

static char* malloc_align(uint32_t size, uint32_t align)
{
	if(head == 0) return NULL;

	if(align % 8 != 0) return 0;

	// make size a multiple of 8
	size += (8 - size % 8)%8;

	if(size == 0)
		return NULL;

	// head points to the end of memory
	if(head == MemoryEnd)
		return NULL;

	// the pointer to the block before
	uint32_t* block_before = &head;
	// the pointer to the current block
	uint32_t* current_block = (uint32_t*) head;
	// the size of current block
	uint32_t current_size = *(current_block+1);

	#define curr_blk_align math_cielm((uint32_t)current_block, align)
	#define avl_sz() (current_size - (curr_blk_align - (uint32_t)current_block))

	// loop untill first fit or end
	while(avl_sz() < size && *current_block != MemoryEnd){
		block_before = current_block;
		current_block = (uint32_t*)(*current_block);
		current_size = *(current_block+1);
	}

	if(avl_sz() == size){
		// region desired covers all found region; remove current block
		if(current_size == size)
		{
			*block_before = *current_block;
			return (char*) current_block;
		}
		// region desired covers end of found region, shrink found region
		else if(current_size > size)
		{
			*(current_block + 1) = current_size - size;
			return (char*) curr_blk_align;
		}
	}else if (avl_sz() > size){
		// region desired covers start of found region , create new block
		if(curr_blk_align == (uint32_t)current_block){
			uint32_t* new_block = (uint32_t*)((uint32_t) current_block + size);
			*new_block = *current_block;
			*(new_block+1) = current_size - size;
			*block_before = (uint32_t) new_block;
			return (char*) curr_blk_align;
		}
		// region desired covers some inbetween found region, shrink & create new block
		else
		{
			uint32_t* new_block = (uint32_t*)((uint32_t) curr_blk_align + size);
			*new_block = *current_block;
			*(new_block+1) = avl_sz() - size;
			*current_block = (uint32_t) new_block;
			*(current_block + 1) = curr_blk_align - (uint32_t) current_block;
			return (char*) curr_blk_align;
		}
	}
	// no size fit found
	return NULL;
}

char* alloc_align(uint32_t size, uint32_t align){
	mrlock();
	char* ofs = malloc_align(size, align);
	mflock();
	return ofs;
}

static void mfree(char* ptr, uint32_t size)
{
	uint32_t* free_block = (uint32_t*)ptr;
	
	if(head == 0) return;

	if(free_block == NULL || size == 0)
		return;

	if((uint32_t)free_block < MemoryBeginAddress || (uint32_t)free_block >= MemoryEnd)
		return;

	if((uint32_t)free_block + (uint64_t)size >= MemoryEnd){
		return;
	}

	// define pntr
	uint32_t* current_block;
	uint32_t current_block_size;
	uint32_t* next_block;

	// make size a multiple of 8
	size += (8 - size % 8)%8;


	// the pointer to the current block
	current_block = &head;

	// loop untill block just before the free block
	while (*current_block < (uint32_t)free_block)
	{
		current_block = (uint32_t*)(*current_block);
	}

	// free block is the first
	if(current_block == &head)
	{ 
		// head points to the end
		if(head == MemoryEnd)
		{
			// free block not exceed memory end
			if((uint32_t)free_block + size <= MemoryEnd){
				*free_block  = *current_block;
				*(free_block+1) = size;
				*current_block = (uint32_t)free_block;
			}
		}
		// head points to a block
		else
		{
			next_block = (uint32_t*)(*current_block);
			if((uint32_t)free_block + size == (uint32_t)next_block)
			{
				// merge with next block
				*free_block  = *next_block;
				*(free_block+1) = size + *(next_block+1);
				*current_block = (uint32_t)free_block;
			}
			// free block not interset next block
			else if((uint32_t)free_block + size < (uint32_t)next_block)
			{
				*free_block  = *current_block;
				*(free_block+1) = size;
				*current_block = (uint32_t)free_block;
			}
		}
	}
	// free block not the first
	else
	{
		current_block_size = *(current_block+1);
		next_block = (uint32_t*)(*current_block);
		if((uint32_t)current_block + current_block_size == (uint32_t)free_block)
		{
			// merge with current block
			if((uint32_t)free_block + size == (uint32_t)next_block)
			{
				// merge with next block
				*current_block = *next_block;
				*(current_block+1) = current_block_size + size + *(next_block+1);
			}
			// free block not interset next block
			else if((uint32_t)free_block + size < (uint32_t)next_block)
			{
				*(current_block+1) = current_block_size + size;
			}
		}
		// free block not interset with current block
		else if((uint32_t)current_block + current_block_size < (uint32_t)free_block){
			if((uint32_t)free_block + size == (uint32_t)next_block)
			{
				// merge with next block
				*free_block  = *next_block;
				*(free_block+1) = size + *(next_block+1);
				*current_block = (uint32_t)free_block;
			}
			// free block not interset next block
			else if((uint32_t)free_block + size < (uint32_t)next_block)
			{
				*free_block  = *current_block;
				*(free_block+1) = size;
				*current_block = (uint32_t)free_block;
			}
		}
	}
	
}

void free(char* ptr, uint32_t size){
	mrlock();
	mfree(ptr, size);
	mflock();
}