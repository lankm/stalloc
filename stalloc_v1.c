/* stalloc = static allocation
 *
 */

#include <stdlib.h>

#ifndef STALLOC
#define STALLOC

typedef struct node {
  // Treated as pointers
  size_t next;
  size_t prev : sizeof(size_t)*8-1;

  size_t open : 1;
} Node;

// Allocated memory Node. Treated as pointer
size_t st_mem;


#include <stdio.h> //delete later
void st_print();

// init & destroy
void st_init(size_t size)
{
  // byte array setup
  st_mem = (size_t)malloc(size);

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
void st_destroy()
{
  free((void*)st_mem);
}

// utilities
void st_split(Node *split, Node *new)
{
  // ptr to node after new
  Node *next = ((Node*)(split->next));

  // old <- new -> next
  new->next = split->next;
  new->prev = (size_t)split;

  // new <- next
  next->prev = (size_t)new;

  // split -> new
  split->next = (size_t)new;
}
void st_join(Node *join, Node *rem)
{
  // ptr to node after rem
  Node *next = (Node*)(rem->next);

  // joined -> next
  join->next = rem->next;

  // joined <- next
  next->prev = (size_t)join;
}
void * st_nodeToPtr(Node *n)
{
  return (void*)(((size_t)n)+sizeof(Node));
}
Node * st_ptrToNode(void *ptr)
{
  return (Node*)(((size_t)ptr)-sizeof(Node));
}

// alloc (first), free & sizeof
void *st_alloc(size_t size)
{
  // start from beginning
  Node *b = ((Node*)(st_mem));
  do
  {
    // if open
    if( b->open )
    {
      size_t b_size = (b->next-(size_t)b) - sizeof(Node);

      // if split needed
      if( size < b_size )
      {
        // inserting new node
        Node* new = (Node*)(((size_t)b)+size+sizeof(Node));
        st_split(b, new);

        // changing open values
        new->open=1;
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
}
int st_free(void *ptr)
{
  // get relevent nodes
  Node *n = st_ptrToNode(ptr), 
       *prev = ((Node*)(n->prev)), 
       *next = ((Node*)(n->next));

  // merging adjacent free blocks
  if(next->open)
    st_join(n, next);
  if(prev->open)
    st_join(prev, n);
  else
    n->open = 1;
}
size_t st_sizeof(void *ptr)
{
  // finding coresponding Node
  Node *b = st_ptrToNode(ptr);

  // retrieving value
  return b->next-(size_t)b-sizeof(Node);
}

//delete later
void st_print()
{
  Node *b = ((Node*)(st_mem));
  do {
    printf("==%llu==%llu==\n", (size_t)b, st_sizeof( (void*) (((size_t)b)+sizeof(Node)) ));
    printf("n: %llu, p: %llu, o: %llu\n\n", b->next, b->prev, b->open);

    b = (Node*)(b->next);
  } while (b->next != (size_t)b);
  printf("==%llu==%llu==\n", (size_t)b, 0);
  printf("n: %llu, p: %llu, o: %llu\n\n", b->next, b->prev, b->open);
}

#endif
