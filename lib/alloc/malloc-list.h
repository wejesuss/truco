#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

/// @brief A list of Memory allocations
typedef struct mem_alloc_list
{
  unsigned int quantity;
  unsigned int position;
  void **list;
} alloc_list;

alloc_list get_malloc_list(unsigned int quantity);
void expand_malloc_list(alloc_list *malloc_list);
void add_to_malloc_list(alloc_list *malloc_list, void *ptr);
void free_malloc_list_members(alloc_list *malloc_list);
void unset_malloc_list(alloc_list *malloc_list);
