#include "memlib.h"
#include "syscall_map.h"
#include "rlock.h"

static uint64_t mhead;
static uint64_t MemoryBeginAddress;
static uint64_t MemoryEnd;
static void* lock;

void minit()
{
   	asm("mov %0, %%rsi\n\t""int $0x80":: "i"(MEMORY_INIT_INT));
    asm("mov %%rbx, %0" : "=b"(MemoryBeginAddress), "=d"(MemoryEnd));

	mhead = MemoryBeginAddress;
	uint64_t* headpntr = (uint64_t*) mhead;
	*headpntr = MemoryEnd;
	*(headpntr+1) = MemoryEnd - MemoryBeginAddress;
}

static void* alloc(uint64_t size)
{
	if(mhead == 0) return NULL;

	// make size a multiple of 16
	size += (16 - size % 16)%16;

	if(size == 0)
		return NULL;

	// mhead points to the end of memory
	if(mhead == MemoryEnd)
		return NULL;

	// the pointer to the block before
	uint64_t* block_before = &mhead;
	// the pointer to the current block
	uint64_t* current_block = (uint64_t*) mhead;
	// the size of current block
	uint64_t current_size = *(current_block+1);

	// loop untill first fit or end
	while(current_size < size && *current_block != MemoryEnd){
		block_before = current_block;
		current_block = (uint64_t*)(*current_block);
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
		uint64_t* new_block = (uint64_t*)((uint64_t) current_block + size);
		*new_block = *current_block;
		*(new_block+1) = current_size - size;
		*block_before = (uint64_t) new_block;
		return (char*) current_block;
	}
	// no size fit found
	return NULL;
}

static void free(void* ptr, uint64_t size)
{
	uint64_t* free_block = (uint64_t*)ptr;
	
	if(mhead == 0) return;

	if(free_block == NULL || size == 0)
		return;

	if((uint64_t)free_block < MemoryBeginAddress || (uint64_t)free_block >= MemoryEnd)
		return;

	if((uint64_t)free_block + (uint64_t)size >= MemoryEnd){
		return;
	}

	// define pntr
	uint64_t* current_block;
	uint64_t current_block_size;
	uint64_t* next_block;

	// make size a multiple of 16
	size += (16 - size % 16)%16;


	// the pointer to the current block
	current_block = &mhead;

	// loop untill block just before the free block
	while (*current_block < (uint64_t)free_block)
	{
		current_block = (uint64_t*)(*current_block);
	}

	// free block is the first
	if(current_block == &mhead)
	{ 
		// mhead points to the end
		if(mhead == MemoryEnd)
		{
			// free block not exceed memory end
			if((uint64_t)free_block + size <= MemoryEnd){
				*free_block  = *current_block;
				*(free_block+1) = size;
				*current_block = (uint64_t)free_block;
			}
		}
		// mhead points to a block
		else
		{
			next_block = (uint64_t*)(*current_block);
			if((uint64_t)free_block + size == (uint64_t)next_block)
			{
				// merge with next block
				*free_block  = *next_block;
				*(free_block+1) = size + *(next_block+1);
				*current_block = (uint64_t)free_block;
			}
			// free block not interset next block
			else if((uint64_t)free_block + size < (uint64_t)next_block)
			{
				*free_block  = *current_block;
				*(free_block+1) = size;
				*current_block = (uint64_t)free_block;
			}
		}
	}
	// free block not the first
	else
	{
		current_block_size = *(current_block+1);
		next_block = (uint64_t*)(*current_block);
		if((uint64_t)current_block + current_block_size == (uint64_t)free_block)
		{
			// merge with current block
			if((uint64_t)free_block + size == (uint64_t)next_block)
			{
				// merge with next block
				*current_block = *next_block;
				*(current_block+1) = current_block_size + size + *(next_block+1);
			}
			// free block not interset next block
			else if((uint64_t)free_block + size < (uint64_t)next_block)
			{
				*(current_block+1) = current_block_size + size;
			}
		}
		// free block not interset with current block
		else if((uint64_t)current_block + current_block_size < (uint64_t)free_block){
			if((uint64_t)free_block + size == (uint64_t)next_block)
			{
				// merge with next block
				*free_block  = *next_block;
				*(free_block+1) = size + *(next_block+1);
				*current_block = (uint64_t)free_block;
			}
			// free block not interset next block
			else if((uint64_t)free_block + size < (uint64_t)next_block)
			{
				*free_block  = *current_block;
				*(free_block+1) = size;
				*current_block = (uint64_t)free_block;
			}
		}
	}
	
}

static uint64_t get_rsp(){
	uint64_t rsp;
	asm("mov %%rsp, %0": "=d"(rsp));
	return rsp;
}

#define mrlock() resource_lock_request(&lock, (void*)(get_rsp()))
#define mflock() resource_lock_free(&lock, (void*)(get_rsp()))

void* malloc(uint64_t size){
	mrlock();
	void* ofs = alloc(size);
	mflock();
	return ofs;
}

void mfree(void* ptr, uint64_t size){
	mrlock();
	free(ptr, size);
	mflock();
}


void* operator new(uint64_t size) {
    void* p = malloc(size);
    return p;
}

// Overloading the global delete operator
void operator delete(void* p, uint64_t sz) {
    mfree(p, sz);
}

void* operator new[](uint64_t size) {
    void* p = malloc(size);
    return p;
}

// Overloading the global delete operator
void operator delete[](void* p, uint64_t sz) {
    mfree(p, sz);
}