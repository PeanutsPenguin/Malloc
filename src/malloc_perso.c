#include "malloc_perso.h"

void* firstblock = NULL;

/*main malloc function*/
void*       malloc_perso(size_t size)
{ 
   if (size <= 0)
   {
      return NULL;
   }

   size_t newSize = aligment(size);          //In First, it align the size on 16 

   t_block* block = find_block(newSize);  //In Second, it search for a free block

   if (block == NULL)                     //If it can't find any block, it'll create one
   {
      block = extend_heap(newSize);
   }

   block->m_blockSize = newSize;

   
	return block + 1;
}
/*function to initialize a block of the struct s_block*/
void        initialize_block(t_block* block)
{
   block->m_free = false;                       //If we CREATE a block it can't be free
   block->m_next = NULL;                        //If we CREATE a block it's the last one of the list so his next is NULL
   if (firstblock == NULL)                      //If the gobal variable first block is NULL it means the block we're creating 
   {
      firstblock = block; 
      block->m_prev = NULL;
   }
   else
   {
      t_block* temp = (t_block *)firstblock;    //Create a temporary block that takes the adress of the first one
      do
      {
         block->m_prev = temp;                  //Set the previous of our actual block to the temporary block
         temp = temp->m_next;                   //set out temporary block to the next one
      }
      while(temp != NULL);                      //Doing these two steps while the temporary block is different to NULL
      block->m_prev->m_next = block;            //Take the previous block and set the next to our actual block 
   }
} 
/*create space in the heap to our block + ou data*/
t_block*    extend_heap(size_t size)
{
   t_block* block = sbrk(0);                       //The block we're about to create will take the adress of where we currently are in the heap

   if (sbrk(sizeof(t_block) + size) == (void*)-1)  //Pushing the heap limit to create space to our block + our data
    	return NULL;                                 //If the new address of the heap limit doesn't exist we return NULL

   initialize_block(block);                        //Create the block

   printf("I'm going to alloc %ld bytes (sizeof t_block: %ld + requested size %ld (alignement 16))\n"
   ,size + sizeof(t_block), sizeof(t_block), size);
   printf("The pointer to the block is %p / block end is %p / so aligned data is at %p \n",
   (void*)block, (void*)(block +1), (void*)(block + 1));
   printf("pointer to the prev one : %p \n", (void*)block->m_prev);

   return block; 
}

/*Align the size that we receive to a multiple of 16*/
size_t         aligment(size_t size)
{
   size_t numToAlloc = size % 16;                  //Test if the number is divisible by 16, if not it return the rest         

   if(numToAlloc != 0)                          //If the rest of the modulo 16 is not equal to zero, alignment is required
   {
      numToAlloc = size + (16 - numToAlloc);    //The new size we have to alloc in the heap equal to our actual size + the rest of the modulo 16
   }
   else
   {
      numToAlloc = size;
   }
   
   return numToAlloc;                           //returnin the new Size to Alloc
}

/*Try to find a free block with the same or more size we send*/
void*       find_block(size_t size)
{
   t_block * temp = (t_block*)firstblock;                   //Create a temporary block that takes the adress of the first one 

   if(temp == NULL)
      return NULL;

   do 
   {
      if(temp->m_free ==  true && (size_t)temp->m_blockSize >= size)        //If a block is free and it has the right size then :
      {
         printf("I found free block %p \n", (void*)temp);
         if((size_t)temp->m_blockSize > size + sizeof(t_block) + 16)           //If the block has a size than can stock our data + a new block with a minimum data then :
         {
            split_block(temp, size);                                 //Splitting the block to make it the right size
         }
         temp->m_free = false;                                    //Else it means we just have space for our data so we set free to false
         return temp;                                             //So we're returning the adress of the data
      }
      temp = temp->m_next;                                     //Setting our temporary block to the next one
   }
   while(temp != NULL);                                     //Do all these steps above while our temporary block is different to NULL

   return NULL;
}
/*Split a block in two to save space*/
void        split_block(t_block* block, size_t size)
{
   size_t sizeOfNewBlock =  block->m_blockSize - size;                        //Initialize the size of our futur new Block          
   printf("Block %p of size %ld ",
   (void*)block, block->m_blockSize);

   block->m_blockSize = size;                                                 //The size of our current block equal to the size we want to alloc
   block->m_free = false;                                                     //The block is now taken so we set the free to false 
   t_block* newBlock = (t_block*)((char*)block + sizeof(t_block) + size);     //Create a new block at the address after the previous block + his data
   newBlock->m_prev = block;                                                  //The previous block of our new block equal to our current block 
   newBlock->m_blockSize = sizeOfNewBlock - sizeof(t_block);                  //The size of our new block equal to the new block size we initialize just before 
   newBlock->m_free = true;                                                   //We don't put any data in this new block so it's free
   newBlock->m_next = block->m_next;                                          //The next block of our new block equal to next block of our current block
   if (block->m_next == NULL)
      newBlock->m_next = NULL;
   block->m_next = newBlock;                                                  //As we change the next block of the new block we also have to change the nextr block of our current block

   printf("can be split when allocating %ld bytes because block size > minimumSplitBlockSize \n",
   size);
   printf("Splitting Block %p to make it size %ld and creating new block %p having size %ld.\n",
   (void*)block, size, (void*)newBlock, sizeOfNewBlock);
   printf("the next block : %p  of this block %p should be the this one : %p \n",
   (void*)block->m_next, (void*)block, (void*)newBlock);
}

void heap_dump(void)
{
    t_block * current = firstblock;
    while (current != NULL)
    {

        if (current->m_free) printf("| An %ldb Chunk | Address %p | Available\n", current->m_blockSize, (void*)current);
        else printf("| An %ldb Chunk | Address %p | Ocupied\n", current->m_blockSize, (void*)current);

        current = current->m_next;
    }
}

void*        calloc_perso(size_t nbrOfElements, size_t size)
{
   return (void*)malloc_perso(size * nbrOfElements);
}
