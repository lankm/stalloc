/* stalloc = static allocation
 *
 */

#include <stdlib.h>
#include <stdint.h>

typedef struct block {
  size_t size;
  
  void *next;
  void *prev;

  //open?
} Block;

// allocated memory block
void *st_mem;


// init & destroy
void st_init(size_t size)
{
  

}
void st_destroy()
{
  // if initiallized
  if(st_mem)
    free(st_mem);
}

// alloc & free
void *st_alloc(size_t size)
{
  return NULL;
}
int st_free(void *ptr)
{
  return 0;
}
