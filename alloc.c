#include <stdlib.h>
#include <stdio.h>
#define HEAP_SIZE 2048
#define true 1
#define false 0

typedef int bool;

typedef struct header
{
    size_t size;
    struct header* next;
    struct header* prev;
    bool is_freed;
} header_t;

static char heap[HEAP_SIZE];                //Allocated memroy
static char* heap_pointer = heap;           //Pointer on the heap top
static header_t* allocated_list = NULL;     //list of allocated memory

void* mem_alloc(size_t size)
{
    header_t* segment = allocated_list;
    while(segment)
    {
        if (segment->is_freed && segment->size >= size)
        {
            printf("Retrun used segment of %d bytes\n", segment->size);
            segment->is_freed = false;
            return ++segment;
        }
        if (segment->next == NULL)
        { break; }
        segment = segment->next;
    }

    //Is enough space in heap
    if ((HEAP_SIZE - (heap_pointer - heap)) >= size + sizeof(header_t))
    {
        header_t* alloc_memory = (header_t*) heap_pointer;
        heap_pointer += size + sizeof(header_t);
        alloc_memory->size = size;
        alloc_memory->is_freed = false;
        alloc_memory->next = NULL;
        alloc_memory->prev = segment;

        if (!allocated_list)
        { allocated_list = alloc_memory; }
        else
        { segment->next = alloc_memory; }

        printf("Allocated %d bytes of memory\n", alloc_memory->size, heap_pointer - heap);

        return ++alloc_memory;
    }

    printf("ERROR: MEMORY ALLOCATION FAULT\n");
    return NULL;
}

int mem_free(void* mem_address)
{
    if (heap_pointer - (char*)mem_address <= 0 || heap_pointer - (char*)mem_address > HEAP_SIZE - sizeof(header_t))
    {
        printf("ERROR: INCORRECT MEMORY ADDRESS\n");
        return -1;
    }
    header_t* header = (header_t*)mem_address;
    --header;
    if (header->is_freed)
    {
        printf("ERROR: MEMORY ALREADY FREED\n");
        return -1;
    }

    int freed_memory = 0;
    if (heap_pointer == (char*)(header + 1) + header->size)
    {
        do
        {
            freed_memory += header->size;
            heap_pointer -= header->size;
            heap_pointer -= sizeof(header_t);
            if (!header->prev)
            { break; }
            header->prev->next = NULL;
            header = header->prev;
        }
        while (header->is_freed);
        printf("%d bytes freed by decrementing heap pointer\n", freed_memory);
        return 0;
    }

    header->is_freed = true;
    if (header->prev)
    { header->prev->next = NULL; }
    printf("Freed %d bytes segment\n", header->size);
    return 0;
}

int main()
{

    long* lnum = mem_alloc(sizeof(long));
    int* num = mem_alloc(sizeof(int));
    long* lnum2 = mem_alloc(sizeof(long));
    mem_free(lnum);
    int* num2 = mem_alloc(sizeof(int));
    mem_free(lnum2);
    mem_free(num2);
    mem_free(num);
    return 0;
}
