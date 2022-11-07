/* stalloc = static allocation
 *
 */

#include <stdlib.h>

#ifndef STALLOC
#define STALLOC

// defines for perfomance sake
#ifndef STALLOCpp
#define SYS_BYTE   sizeof(void*)
#define ALIGN(s)   ((s-1) / SYS_BYTE * SYS_BYTE) + SYS_BYTE
#define st_nodeToPtr(n)    (void*) (  ((size_t)n)  + sizeof(Node) )
#define st_ptrToNode(ptr)  (Node*) ( ((size_t)ptr) - sizeof(Node) )

#endif

typedef struct node {
  // Treated as pointers
  size_t next;
  size_t prev : sizeof(size_t)*8 - 1; // better memory but 5% reduction in performance

  size_t open : 1;
} Node;

// Allocated memory Node. Treated as pointer
size_t st_mem;

// init & destroy
void st_init(size_t size)
{
  size = ALIGN(size);

  // byte array setup
  st_mem = (size_t) malloc(size);
  
  // head tail locations
  Node *head = ((Node*)(st_mem));
  Node *tail = ((Node*)(st_mem+size-sizeof(Node)));

  // Head Node setup
  head->next = (size_t)tail;
  head->prev = (size_t)head;
  head->open = 1;

  // tail Node setup
  tail->next = (size_t)tail;
  tail->prev = (size_t)head;
  tail->open = 0;
}
#define st_destroy() {\
  free((void*)st_mem);\
}

// utilities
#define st_split(split, create) {\
  create->next = split->next;\
  create->prev = (size_t)split;\
  ((Node*)(split->next))->prev = (size_t)create;\
  split->next = (size_t)create;\
}
#define st_join(join, rem) {\
  join->next = rem->next;\
  ((Node*)(rem->next))->prev = (size_t)join;\
}

// alloc (first), free & sizeof
void *st_malloc(size_t size)
{
  // No readable version needed. Already as fast as possible
  size = ALIGN(size);

  // start from beginning
  Node *b = ((Node*)(st_mem));
  do
  {
    // if open
    if( b->open )
    {
      // Size = distance - size of node
      size_t b_size = ( b->next - (size_t)b ) - sizeof(Node);

      // if split needed
      if( size < b_size )
      {
        // inserting create node
        Node* create = (Node*)(((size_t)b) + size + sizeof(Node));
        st_split(b, create);

        // changing open values
        create->open=1;
        b->open=0;

        return st_nodeToPtr(b);
      }
      // if no split needed
      else if( size == b_size)
      {
        // convert current node
        b->open=0;

        return st_nodeToPtr(b);
      }
    }

    // goto next Node
    b = (Node*)(b->next);
  } while (b->next != (size_t)b);

  return NULL;
}
#define st_free(ptr) {\
  if( ((Node*)((st_ptrToNode(ptr))->next))->open )\
    {st_join(         (st_ptrToNode(ptr))         , ((Node*)((st_ptrToNode(ptr))->next)) );}\
  if( ((Node*)((st_ptrToNode(ptr))->prev))->open )\
    {st_join( ((Node*)((st_ptrToNode(ptr))->prev)),         (st_ptrToNode(ptr))          );}\
  else\
    (st_ptrToNode(ptr))->open = 1;\
}
#define st_sizeof(ptr) {\
  (st_ptrToNode(ptr))->next - (size_t)(st_ptrToNode(ptr)) - sizeof(Node)\
}

#endif
