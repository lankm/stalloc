/* stalloc = static allocation
 *
 */

#include <stdlib.h>
//delete later
#include <stdio.h>

#ifndef STALLOC
#define STALLOC

#define SYS_BYTE      sizeof(void*)
#define ALIGN(s)      ((s-1) / SYS_BYTE * SYS_BYTE) + SYS_BYTE
#define st_indexOf(p) (size_t) ((char*)p - (char*)st_mem)

typedef struct node {
  // Indexes
  size_t next;
  size_t prev : sizeof(size_t)*8-1; 

  size_t open : 1;
} Node;

// Allocated memory as byte array.
char *st_mem;

// init & destroy
void st_init(size_t size)
{
  // Alligning
  size = ALIGN(size);

  // Byte array init
  st_mem = (char*) malloc(size);

  // Indexes
  size_t head_i = 0;
  size_t tail_i = size-sizeof(Node);
  // Ptrs
  Node *head = (Node*) &st_mem[ head_i ];
  Node *tail = (Node*) &st_mem[ tail_i ];

  // Head Node setup
  head->next = tail_i;
  head->prev = head_i;
  head->open = 1;

  // Tail Node setup
  tail->next = tail_i;
  tail->prev = head_i;
  tail->open = 0;
}
void st_destroy()
{
  free(st_mem);
}

// utilities
void st_split(Node *split, Node *create)
{
  // ptr to node after 'create'
  Node *next = (Node*) &st_mem[ split->next ];

  // split <- create -> next
  create->next = st_indexOf(next);
  create->prev = st_indexOf(split);

  // split -> create <- next
  size_t create_i = st_indexOf(create);
  split->next = create_i;
  next->prev = create_i;
}
void st_join(Node *join, Node *remove)
{
  // ptr to node after rem
  Node *next = (Node*) &st_mem[ remove->next ];

  // join <-> next
  join->next = remove->next;
  next->prev = remove->prev;
}
// Convert to defines
void * st_nodeToPtr(Node *n)
{
  // Go forward by the size of node
  return (void*) &n[1];
}
Node * st_ptrToNode(void *ptr)
{
  // casting because gcc warns about dereferencing void pointers
  char *ptr2 = (char*)ptr; 

  // Go back by the size of node
  return (Node*) &ptr2[-sizeof(Node)];
}
size_t st_sizeof(void *ptr)
{
  // Retrieving coresponding Node
  Node *n = st_ptrToNode(ptr);

  // Distance to next node - size of Node
  return n->next - st_indexOf(n) - sizeof(Node);
}
// alloc (first), free & sizeof
void* st_malloc(size_t size)
{
  size = ALIGN(size);

  // Start from beginning
  Node *n = (Node*) st_mem;
  do
  {
    // If open
    if(n->open)
    {
      size_t n_size = st_sizeof(n) - sizeof(Node);

      // If split needed
      if( size < n_size)
      {
        // make 'create' after given block
        size_t create_i = st_indexOf(n) + size + sizeof(Node);
        Node *create = (Node*) &st_mem[ create_i];
        st_split(n, create);

        // changing open values
        n->open = 0;
        create->open = 1;

        return st_nodeToPtr(n);
      }
      // If no split needed
      else if( size == n_size)
      {
        // convert current node
        n->open = 0;

        return st_nodeToPtr(n);
      }
    }

    // goto next Node
    n = (Node*) &st_mem[ n->next ];
  } while ( st_indexOf(n) != n->next);

    // No block found
  return NULL;
}
void st_free(void *ptr)
{
  // get relevent nodes
  Node *n = st_ptrToNode(ptr), 
       *prev = (Node*) &st_mem[ n->prev ], 
       *next = (Node*) &st_mem[ n->next ];

  // merging adjacent free blocks
  if(next->open)
    st_join(n, next);
  if(prev->open)
    st_join(prev, n);
  else
    n->open = 1;
}

//delete later
void st_printN(Node *n)
{
  // Most usefull info
  void *ptr = st_nodeToPtr(n);
  printf("Node:<%p>| Size: %llu\n", n, st_sizeof(ptr));

  // Node variables
  printf("Next: %p |\n", &st_mem[ n->next ]);
  printf("Prev: %p | ", &st_mem[ n->prev ]);
  printf("Open: %d\n", n->open);
}
void st_print()
{
  Node *n = (Node*) st_mem;
  do
  {
    // Print
    st_printN(n);
    printf("\n");

    // Go to next
    n = (Node*) &st_mem[ n->next ];

    // While next isn't itself
  } while ( n != (Node*)&st_mem[ n->next ] );

  // Print last node
  // last node underflows its size because it has none and is last
  st_printN(n);
}

#endif
