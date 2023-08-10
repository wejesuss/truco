#include "./malloc-list.h"

/// @brief Create and return a new malloc list.
/// @param quantity Initial number of spaces available (for addresses)
alloc_list get_malloc_list(unsigned int quantity)
{
  alloc_list malloc_list = {.position = 0, .list = NULL, .quantity = quantity};
  malloc_list.list = malloc(sizeof(void *) * quantity);
  malloc_list.quantity = quantity;

  printf("%p\n", malloc_list.list);

  return malloc_list;
}

void expand_malloc_list(alloc_list *malloc_list)
{
  unsigned int size = (malloc_list->quantity * 1.5);
  malloc_list->list = realloc(malloc_list->list, sizeof(void *) * size);
  malloc_list->quantity = size;
}

void add_to_malloc_list(alloc_list *malloc_list, void *ptr)
{
  unsigned int pos = malloc_list->position;
  bool expand = (pos + 1) == malloc_list->quantity;
  if (expand)
  {
    expand_malloc_list(malloc_list);
  }

  malloc_list->list[pos] = ptr;
  malloc_list->position++;
}

void free_malloc_list_members(alloc_list *malloc_list)
{
  for (unsigned int i = 0; i < malloc_list->position; i++)
  {
    free(malloc_list->list[i]);
  }

  malloc_list->position = 0;
}

void unset_malloc_list(alloc_list *malloc_list)
{
  free_malloc_list_members(malloc_list);
  free(malloc_list->list);
  malloc_list->quantity = 0;
  malloc_list->list = NULL;
}
