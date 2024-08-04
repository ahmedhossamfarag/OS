#include "memory.h"
#include "rlock.h"
#include "info.h"

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

static void mfree(char* ptr, uint32_t size)
{
	uint32_t* free_block = (uint32_t*)ptr;
	
	if(head == 0) return;

	if(free_block == NULL || size == 0)
		return;

	if((uint32_t)free_block < MemoryBeginAddress || (uint32_t)free_block >= MemoryEnd)
		return;

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
			*free_block  = *current_block;
			*(free_block+1) = size;
			*current_block = (uint32_t)free_block;
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
			else
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
			else
			{
				*(current_block+1) = current_block_size + size;
			}
		}
		else if((uint32_t)free_block + size == (uint32_t)next_block)
		{
			// merge with next block
			*free_block  = *next_block;
			*(free_block+1) = size + *(next_block+1);
			*current_block = (uint32_t)free_block;
		}
		else
		{
			*free_block  = *current_block;
			*(free_block+1) = size;
			*current_block = (uint32_t)free_block;
		}
	}
	
}

void free(char* ptr, uint32_t size){
	mrlock();
	mfree(ptr, size);
	mflock();
}