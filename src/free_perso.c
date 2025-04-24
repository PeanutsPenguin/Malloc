#include "free_perso.h"

extern void* firstblock;

void    free_perso(void* ptr)
{
    t_block* temp = firstblock;

    if (temp == NULL || ptr == NULL)
    {
        return;
    }

    t_block* block = (void*)((char*)ptr - sizeof(t_block));

    do  
    {
        // printf("Trying to Free block %p\n", block);
        // printf("Bloc i compare to : %p\n", temp);
        if (block == temp)        
        {
            printf("Freeing block %p\n", (void*)block);
            // printf("the previous block is : %p \n", block->m_prev);
            block->m_free = true; 
        }
        temp =  temp->m_next;   
    } 
    while(temp !=  NULL);

    if (block->m_next != NULL || block->m_prev != NULL)
    {
        try_to_fusion(block);
    }
    
}

void    try_to_fusion(t_block* block)
{
    if (block->m_next != NULL)
    {
        if (block->m_next->m_free == true)
        {
            printf("Merging block %p  (size : %ld) with the next one %p (size : %ld) because both are free\n", (void*)block, block->m_blockSize, (void*)block->m_next, block->m_next->m_blockSize);
            if (block->m_next->m_next != NULL)
            {
                block->m_next->m_next->m_prev = block;
            }
            block->m_blockSize = block->m_blockSize + sizeof(t_block) + block->m_next->m_blockSize;
            block->m_next = block->m_next->m_next;
            printf("Block size is now %ld \n", block->m_blockSize);
        }
    }

    if (block->m_prev != NULL)
    {
        if (block->m_prev->m_free == true)
        {
            printf("Merging block %p  (size : %ld) with the previous one %p (size : %ld) because both are free\n", (void*)block, block->m_blockSize, (void*)block->m_prev, block->m_prev->m_blockSize);
            block->m_prev->m_next = block->m_next;
            block->m_prev->m_blockSize = block->m_prev->m_blockSize + 32 + block->m_blockSize;
            printf("Block size is now %ld \n", block->m_prev->m_blockSize);
        }
    }
}