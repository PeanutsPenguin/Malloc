#pragma once 
#include "stdbool.h"
#include <stdio.h>
#include <unistd.h>

typedef struct s_block
{
    size_t m_blockSize;
    struct s_block* m_next;
    struct s_block* m_prev;
    bool m_free;  

}t_block;

void*       malloc_perso(size_t size);

void        initialize_block(t_block* block);

t_block*    extend_heap(size_t size);

size_t         aligment(size_t size);

void*       find_block(size_t size);

void        split_block(t_block* block, size_t size);

void*        calloc_perso(size_t nbrOfElements, size_t size);

void heap_dump(void);
