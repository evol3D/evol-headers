#pragma once

//should we add alignment ?

#define ev_malloc(size) __ev_malloc_internal(size, __LINE__, __FILE__)
#define ev_free(ptr) __ev_free_internal(ptr)
#define ev_realloc(ptr, size) __ev_realloc_internal(ptr, size)

#define list_leaks() __ev_list_internal()

#ifdef EV_MEMORYTRACKING_IMPLEMENTATION

#include <stdlib.h>
#include <stdio.h>

#include <assert.h>
#include <unordered_map>

struct evAlloc
{
	void* address;

	size_t size;
	size_t line;
	const char* file;
};

std::unordered_map<void*, evAlloc> allocsMap;

void add_alloc(evAlloc newAlloc);
void remove_alloc(evAlloc Alloc);

void* __ev_malloc_internal(size_t size, size_t line, const char* file)
{
	evAlloc newAlloc;

	newAlloc.size = size;
	newAlloc.line = line;
	newAlloc.file = file;
	newAlloc.address = malloc(size);

	assert(size != 0);
	assert(newAlloc.address != NULL);

	add_alloc(newAlloc);

	return newAlloc.address;
}

void __ev_free_internal(void* ptr)
{
	assert(ptr != NULL);
	assert(allocsMap.find(ptr) != allocsMap.end());

	remove_alloc(allocsMap[ptr]);
}

void* __ev_realloc_internal(void* ptr, size_t size)
{
	return NULL;
}

void __ev_list_internal()
{
	if (allocsMap.empty())
	{
		printf("No Leaks!\n");
		return;
	}

	printf("There are %d Leaks!\n", allocsMap.size());
	for (auto& allocation : allocsMap)
		printf("Allocation in line: %zu, in file: %s wasn't freed.\n", allocation.second.line, allocation.second.file);
}

//do we really need those 2 functions: add_alloc & remove_alloc
void add_alloc(evAlloc newAlloc)
{
	allocsMap[newAlloc.address] = newAlloc;
}

void remove_alloc(evAlloc Alloc)
{
	allocsMap.erase(Alloc.address);
	free(Alloc.address);
}

#endif