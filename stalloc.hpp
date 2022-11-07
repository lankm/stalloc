/* stalloc = static allocation
 *
 */

#include <stdlib.h>

#ifndef STALLOCpp
#define STALLOCpp

// defines for perfomance sake
#ifndef STALLOC
#define SYS_BYTE   sizeof(void*)
#define ALIGN(s)   ((s-1) / SYS_BYTE * SYS_BYTE) + SYS_BYTE
#define st_nodeToPtr(n)    (void*) (  ((size_t)n)  + sizeof(Node) )
#define st_ptrToNode(ptr)  (Node*) ( ((size_t)ptr) - sizeof(Node) )
#define st_sizeof(ptr)     (st_ptrToNode(ptr))->next - (size_t)(st_ptrToNode(ptr)) - sizeof(Node)
#endif

namespace sta{}
using namespace sta;

class stalloc
{
private:
  typedef struct node
  {
    // Treated as pointers
    size_t next;
    size_t prev : sizeof(size_t)*8 - 1; // better memory but 5% reduction in performance

    size_t open : 1;
  } Node;

  // Allocated memory Node. Treated as pointer
  size_t st_mem;

  // LL utilities
  static void split(Node *split, Node *create)
  {
    /* readable version
    // ptr to node after create
    Node *next = ((Node*)(split->next));

    // old <- create -> next
    create->next = split->next;
    create->prev = (size_t)split;

    // create <- next
    next->prev = (size_t)create;

    // split -> create
    split->next = (size_t)create;
    */

    // old <- create -> next
    create->next = split->next;
    create->prev = (size_t)split;

    // create <- next
    ((Node *)(split->next))->prev = (size_t)create;

    // split -> create
    split->next = (size_t)create;
  }
  static void join(Node *join, Node *rem)
  {
    /* readable version
    // ptr to node after rem
    Node *next = (Node*)(rem->next);

    // joined -> next
    join->next = rem->next;

    // joined <- next
    next->prev = (size_t)join;
    */

    // joined -> next
    join->next = rem->next;

    // joined <- next
    ((Node *)(rem->next))->prev = (size_t)join;
  }

public:
  stalloc(size_t size)
  {
    
    size = ALIGN(size);

    // byte array setup
    st_mem = (size_t) malloc(size);
    

    // head tail locations
    Node *head = ((Node *)(st_mem));
    Node *tail = ((Node *)(st_mem + size - sizeof(Node)));

    // Head Node setup
    head->next = (size_t)tail;
    head->prev = (size_t)head;
    head->open = 1;

    // tail Node setup
    tail->next = (size_t)tail;
    tail->prev = (size_t)head;
    tail->open = 0;
  }
  ~stalloc()
  {
    free((void *)st_mem);
  }

  // alloc (first), free & sizeof
  void *alloc(size_t size)
  {
    // No readable version needed. Already as fast as possible
    size = ALIGN(size);

    // start from beginning
    Node *b = ((Node *)(st_mem));
    do
    {
      // if open
      if (b->open)
      {
        // Size = distance - size of node
        size_t b_size = (b->next - (size_t)b) - sizeof(Node);

        // if split needed
        if (size < b_size)
        {
          // inserting create node
          Node *create = (Node *)(((size_t)b) + size + sizeof(Node));
          split(b, create);

          // changing open values
          create->open = 1;
          b->open = 0;

          return st_nodeToPtr(b);
        }
        // if no split needed
        else if (size == b_size)
        {
          // convert current node
          b->open = 0;

          return st_nodeToPtr(b);
        }
      }

      // goto next Node
      b = (Node *)(b->next);
    } while (b->next != (size_t)b);

    return NULL;
  }
  void free(void *ptr)
  {
    /* readable version
    // get relevent nodes
    Node *n = st_ptrToNode(ptr),
         *prev = ((Node*)(n->prev)),
         *next = ((Node*)(n->next));

    // merging adjacent free blocks
    if(next->open)
      join(n, next);
    if(prev->open)
      join(prev, n);
    else
      n->open = 1;
    */

    // merging adjacent free blocks
    if ( ((Node *)((st_ptrToNode(ptr))->next))->open )
      join(          (st_ptrToNode(ptr))        , ((Node *)((st_ptrToNode(ptr))->next)));
    if ( ((Node *)((st_ptrToNode(ptr))->prev))->open )
      join(((Node *)((st_ptrToNode(ptr))->prev)),        (st_ptrToNode(ptr))           );
    else
      (st_ptrToNode(ptr))->open = 1;
  }
  size_t size(void *ptr)
  {
    return st_sizeof(ptr);
  }
};

#endif
