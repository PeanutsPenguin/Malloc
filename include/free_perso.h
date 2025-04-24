#pragma once
#include "malloc_perso.h"

typedef struct s_block t_block;

void    free_perso(void* ptr);

void    try_to_fusion(t_block* block);