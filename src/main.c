// #include "malloc_perso.h"
// #include "free_perso.h"

// int main()
// {
//   printf("----------------------------\nSTEP 1: First malloc: no bloc SHOULD EXTEND HEAP\n\n");
//   int* test = (int*)malloc_perso(4096);
//   printf("\nCHECK: Pointer %p is aligned on 16 ? %s\n",
// 	 (void*)test, ((size_t)test % 16) == 0 ? "yes" : "no" );
  
//   test[0] = 1;
//   test[1] = 2;
//   test[2] = 2;

//   printf("\n----------------------------\nSTEP 2 : First free\n\n");
//   free_perso(test);

//   printf("\n----------------------------\nSTEP 3: Second malloc fitting bloc SHOULD SPLIT\n\n");
//   int* test2 = (int*)malloc_perso(16);  
//   test2[0] = 2323;

//   printf("\nCHECK: Pointer test2 should be equal to test (reuse first block): %p = %p\n",
// 	 (void*)test, (void*)test2);
  

//   printf("\n----------------------------\nSTEP 4: third malloc should take new split block\n");
//   int* test3 = (int*)malloc_perso(4030);
//   test3[0] = 111111;

//   printf("\nCHECK: Pointer %p is aligned on 16 ? %s\n",
// 	 (void*)test3, ((size_t)test3 % 4) == 0 ? "yes" : "no" );


//   printf("\n----------------------------\nSTEP 5: Second free should free first block\n\n");
//   free_perso(test2);
  
  
//   printf("\n----------------------------\nSTEP 6: Fourth malloc should take first block without split\n\n");
//   int* test4 = (int*)malloc_perso(12);
//   test4[1] = 3;

//   printf("\nCHECK: Pointer test4 should be equal to test (reuse first block): %p = %p\n",
// 	 (void*)test, (void*)test4);

  
//   printf("\n----------------------------\nSTEP 7: Fifth malloc no available bloc - SHOULD EXTEND HEAP\n\n");
//   int* test5 = (int*)malloc_perso(5555);
//   test5[33] = 3;

//   printf("\nCHECK: Pointer test5 should be a new block (no block reuse): %s\n",
// 	 (test5 != test && test5 != test3) ? "OK" : "FAIL");

//   printf("\nCHECK: Pointer %p is aligned on 4 ? %s\n",
// 	 (void*)test5, ((size_t)test5 % 4) == 0 ? "yes" : "no" );
    
//   printf("\n----------------------------\nSTEP 8: Free the two first blocks - SHOULD FUSION\n");
//   printf("\nFirst free\n\n"); 
//   free_perso(test3);
//   printf("\nSecond free\n\n"); 
//   free_perso(test4);


//   printf("\n----------------------------\nSTEP 9: Final free - should merge\n");
//   free_perso(test5);

//   return 0;

// }

#include <stdio.h> // printf

#include "malloc_perso.h"
#include "free_perso.h"
#include <stdlib.h>
#include <assert.h>
#include <unistd.h> // sbrk
#include <string.h> // memset

#define WANTED_ALIGN 4

#define	MOULINETTE_ALIGN_UP(num, align)		\
  (((num) + ((align) - 1)) & ~((align) - 1))

#define DEFAULT_MIN_STEP 0
#define STEPS_NUMBER 8
#define DEFAULT_MAX_STEPS STEPS_NUMBER


typedef void	(*test_func)();




void check_malloc_block(void* block)
{
  assert (block != NULL && ((size_t)block % WANTED_ALIGN == 0));
}



void	check_memoryblock_use(void* userspace, size_t size)
{
  memset(userspace, 0x2A, size);
}


void	step_one()
{
  // check malloc zero
  printf("\n-------------------------------\nSTEP ONE: malloc(0) should return NULL\n");
 void* null = malloc_perso(0);
 if (null != NULL)
   {
     printf("WARNING: malloc_perso(0) does not return NULL\n");
   }
 else
   {
     printf("OK\n");
   }


}



void	step_two()
{
  printf("\n-------------------------------\nSTEP TWO: Invalid free (NULL and not a block)\n");
  // free from not a block
  free_perso(NULL);

  free_perso((void*)1221);

  // double free

  // a legit malloc
  void* block = malloc_perso(40);
  check_malloc_block(block);
 

  free_perso(block);
  printf("\nDouble free!\n");
  free_perso(block);

}


void step_three()
{

  // malloc 1
  printf("\n-------------------------------\nSTEP THREE: Lots of small mallocs\n");
  void* tests[42];
  for (int iStep = 0; iStep != 42; ++iStep)
    {
      tests[iStep] = malloc_perso(iStep + 1);
      check_malloc_block(tests[iStep]);
      check_memoryblock_use(tests[iStep], iStep + 1);
    }

  // now free everything
  for (int iStep = 0; iStep != 42; ++iStep)
    {
      free_perso(tests[iStep]);
    }
}


void step_four()
{
  // malloc big blocks
  printf("\n-------------------------------\nSTEP FOUR: Malloc big blocks + sequential frees\n");
  void* big1 = malloc_perso(43002);
  check_malloc_block(big1);
  check_memoryblock_use(big1, 43002);

  void* big2 = malloc_perso(32203);
  check_malloc_block(big2);
  check_memoryblock_use(big2, 32203);
  
  void* big3 = malloc_perso(23423);
  check_malloc_block(big3);
  check_memoryblock_use(big3, 23423);
  
  free_perso(big2);
  free_perso(big3);
  free_perso(big1);
}


void step_five()
{
  printf("\n-------------------------------\nSTEP FIVE: Frees out of order\n");
  void* big1 = malloc_perso(3223);
  check_malloc_block(big1);
  check_memoryblock_use(big1, 3223);
  
  void* big2 = malloc_perso(6423);
  check_malloc_block(big2);
  check_memoryblock_use(big2, 6423);

  void* big3 = malloc_perso(4522);
  check_malloc_block(big3);
  check_memoryblock_use(big3, 4522);

  free_perso(big3);
  free_perso(big2);
  free_perso(big1);


}


void step_six()
{

  printf("\n-------------------------------\nSTEP SIX: Interleaved mallocs and frees\n");
  void* big1 = malloc_perso(1023);
  check_malloc_block(big1);
  check_memoryblock_use(big1, 1023);
  free_perso(big1);

  void* big2 = malloc_perso(9346);
  check_malloc_block(big2);
  check_memoryblock_use(big2, 9346);
  free_perso(big2);
  
  void* big3 = malloc_perso(5483);
  check_malloc_block(big3);
  check_memoryblock_use(big3, 5483);
  free_perso(big3);

  big1 = malloc_perso(8734);
  check_malloc_block(big1);
  check_memoryblock_use(big1, 8734);
  
  big2 = malloc_perso(1823);
  check_malloc_block(big2);
  check_memoryblock_use(big2, 1823);
  
  free_perso(big1);

  big3 = malloc_perso(4392);
  check_malloc_block(big3);
  check_memoryblock_use(big3, 4392);
    
  free_perso(big2);

  big1 = malloc_perso(8734);
  check_malloc_block(big1);
  check_memoryblock_use(big1, 8734);
    
  free_perso(big1);

  big1 = malloc_perso(9999);
  check_malloc_block(big1);
  check_memoryblock_use(big1, 9999);

  big2 = malloc_perso(1823);
  check_malloc_block(big2);
  check_memoryblock_use(big2, 1823);

  free_perso(big3);
  free_perso(big2);
  free_perso(big1);
}



void step_seven()
{
  printf("\n-------------------------------\nSTEP SEVEN: Malloc various sizes\n");
  void* big1 = malloc_perso(1023); // not aligned
  check_malloc_block(big1);
  check_memoryblock_use(big1, 1023);
  free_perso(big1);

  void* big2 = malloc_perso(4); // aligned
  check_malloc_block(big2);
  check_memoryblock_use(big2, 4);
  free_perso(big2);
  

  // malloc unaligned size
  void* big3 = malloc_perso(3); // not aligned
  check_malloc_block(big3);
  check_memoryblock_use(big3, 3);
  free_perso(big3);

}



void	step_eight()
{
  printf("\n-------------------------------\nSTEP EIGHT: Testing internal functioning. If this step fails, try to start only this one to clean the block list.\n");
  // Test regular
  // should expand the heap because there's no block
  printf("Testing heap expansion 1\n");
  void* currentBreakPos = sbrk(0);
     heap_dump();
  void* freespace = malloc_perso(2000);
  check_malloc_block(freespace);
  check_memoryblock_use(freespace, 2000);
  

  void* newBreakPos = sbrk(0);
     heap_dump();
  assert(currentBreakPos < newBreakPos); // the break should have increased

  // discover the block list beginning
  t_block* blockList = (t_block*)currentBreakPos;
  assert(blockList->m_next == NULL);
  assert(blockList->m_prev == NULL);
  assert(blockList->m_free == false);

  // do we find our free space at the right aligned address ?
  assert(MOULINETTE_ALIGN_UP((size_t)blockList + sizeof(t_block), WANTED_ALIGN) == (size_t)freespace);
  
  // Free of this single block
  printf("Testing free 1\n");
  free_perso(freespace);
  assert(blockList->m_free == true);

  // realloc something. the first free block should split.
  printf("Testing splitting 1\n");
  freespace = malloc_perso(30);
  check_malloc_block(freespace);
  check_memoryblock_use(freespace, 30);

  // it should be the same block as before
  assert(MOULINETTE_ALIGN_UP((size_t)blockList + sizeof(t_block), WANTED_ALIGN) == (size_t)freespace);

  assert(blockList->m_free == false);

  assert(blockList->m_next != NULL);
  assert(blockList->m_next->m_free == true);
  assert(blockList->m_next->m_prev == blockList);
  assert(blockList->m_next->m_next == NULL);
  assert(blockList->m_next->m_blockSize > blockList->m_blockSize);

  // now use the second splitted block. not enough space to split again
  printf("Testing splitting 2\n");
  void* freespace2 = malloc_perso(1900);
  check_malloc_block(freespace2);
  check_memoryblock_use(freespace2, 1900);

  assert(blockList->m_next != NULL);
  assert(blockList->m_next->m_next == NULL);

  assert(MOULINETTE_ALIGN_UP((size_t)blockList->m_next + sizeof(t_block), WANTED_ALIGN) == (size_t)freespace2);

  // realloc something. the break should change because both blocks are taken
  printf("Testing heap expansion 2\n");
  void* freespace3 = malloc_perso(33);
  check_malloc_block(freespace3);
  check_memoryblock_use(freespace3, 33);
  
  currentBreakPos = newBreakPos;
  newBreakPos = sbrk(0);

  assert(newBreakPos > currentBreakPos);

  assert(MOULINETTE_ALIGN_UP((size_t)blockList->m_next->m_next + sizeof(t_block), WANTED_ALIGN) == (size_t)freespace3);

  printf("Testing block fusion 1: last block should not try to fusion (1/3)\n");
  free_perso(freespace3);

  printf("Testing block fusion 2: first block should not try to fusion (2/3)\n");
  free_perso(freespace);

  printf("Testing block fusion 3: block in the middle should FUSION with previous and next (3/3)\n");
  free_perso(freespace2);

  printf("There should be one single big block at %p\n", (void*)blockList);
    
  assert(blockList->m_free == true);
  assert(blockList->m_next == NULL);
  assert(blockList->m_prev == NULL);


  
  printf("Testing block fusion and split: we should reuse first big block (no heap expansion) and it should split\n");

  currentBreakPos = newBreakPos;

  void* freespace4 = malloc_perso(1500);
  check_malloc_block(freespace4);
  check_memoryblock_use(freespace4, 1500);
  assert(MOULINETTE_ALIGN_UP((size_t)blockList + sizeof(t_block), WANTED_ALIGN) == (size_t)freespace4);
  
  assert(blockList->m_free == false);
  assert(blockList->m_prev == NULL);
  assert(blockList->m_next != NULL);
  assert(blockList->m_next->m_free == true);
  assert(blockList->m_blockSize > blockList->m_next->m_blockSize);
  assert(blockList->m_next->m_next == NULL);
  assert(blockList->m_next->m_prev == blockList);

  newBreakPos = sbrk(0);  
  assert(newBreakPos == currentBreakPos);

  free_perso(freespace4);
}



test_func g_stepsFuncs[STEPS_NUMBER] = {
  &step_one,
  &step_two,
  &step_three,
  &step_four,
  &step_five,
  &step_six,
  &step_seven,
  &step_eight
};



int main(int argc, char **argv)
{
  int wantedMinStep = (argc > 1 ? atoi(argv[1]) : DEFAULT_MIN_STEP);
  wantedMinStep = (wantedMinStep >= STEPS_NUMBER ? STEPS_NUMBER-1 : wantedMinStep);
  wantedMinStep = (wantedMinStep < 0 ? 0 : wantedMinStep);
  
  int wantedMaxStep = (argc > 2 ? atoi(argv[2]) : DEFAULT_MAX_STEPS-1);
  wantedMaxStep = (wantedMaxStep >= STEPS_NUMBER ? STEPS_NUMBER-1 : wantedMaxStep);
  wantedMaxStep = (wantedMaxStep < 0 ? 0 : wantedMaxStep);

  wantedMinStep = (wantedMinStep > wantedMaxStep ? wantedMaxStep : wantedMinStep);

  printf("Testing steps %i to %i\n", wantedMinStep, wantedMaxStep);
  
  for (int iStep = wantedMinStep; iStep <= wantedMaxStep; iStep++)
    {
      g_stepsFuncs[iStep]();
    }
  
  printf("CLEAR!\n");
}