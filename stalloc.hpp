/* stalloc = static allocation
 *
 */

#include <stdlib.h>

#ifndef STALLOCpp
#define STALLOCpp

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
  size_t mem;

  // allignment
  static size_t align(size_t s)
  {
    // get computer bits. 32bit/64bit
    size_t sysBits = sizeof(void*);
    return ((s-1) / sysBits * sysBits) + sysBits;
  }

  // ptr <-> node converison
  static Node* ptrToNode(void *ptr)
  {
    // go back by size of node
    return (Node*) ( ((size_t)ptr) - sizeof(Node) );
  }
  static void* nodeToPtr(Node *n)
  {
    // go forward by size of node
    return (void*) (  ((size_t)n)  + sizeof(Node) );
  }

  // LL utilities
  static void split(Node *split, Node *create)
  {
    // ptr to node after create
    Node *next = ((Node*)(split->next));

    // old <- create -> next
    create->next = split->next;
    create->prev = (size_t)split;

    // create <- next
    next->prev = (size_t)create;

    // split -> create
    split->next = (size_t)create;
  }
  static void join(Node *join, Node *rem)
  {
    // ptr to node after rem
    Node *next = (Node*)(rem->next);

    // joined -> next
    join->next = rem->next;

    // joined <- next
    next->prev = (size_t)join;
  }

public:
  stalloc(size_t size)
  {
    size = align(size);

    // byte array setup
    mem = (size_t) malloc(size);
    

    // head tail locations
    Node *head = ((Node *)(mem));
    Node *tail = ((Node *)(mem + size - sizeof(Node)));

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
    std::free((void *)mem);
  }

  // alloc (first), free & sizeof
  void *alloc(size_t size)
  {
    // No readable version needed. Already as fast as possible
    size = align(size);

    // start from beginning
    Node *b = ((Node *)(mem));
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

          return nodeToPtr(b);
        }
        // if no split needed
        else if (size == b_size)
        {
          // convert current node
          b->open = 0;

          return nodeToPtr(b);
        }
      }

      // goto next Node
      b = (Node *)(b->next);
    } while (b->next != (size_t)b);

    return NULL;
  }
  void free(void *ptr)
  {
    // get relevent nodes
    Node *n = ptrToNode(ptr),
         *prev = ((Node*)(n->prev)),
         *next = ((Node*)(n->next));

    // merging adjacent free blocks
    if(next->open)
      join(n, next);
    if(prev->open)
      join(prev, n);
    else
      n->open = 1;
  }
  size_t size(void *ptr)
  {
    return (ptrToNode(ptr))->next - (size_t)(ptrToNode(ptr)) - sizeof(Node);
  }
};

#endif
