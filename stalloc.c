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
#define st_indexOf(p) (size_t) ((void*)p - (void*)st_mem)

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
  st_mem = malloc(size);

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
void st_split(Node *split, Node *new)
{

}
void st_join(Node *join, Node *rem)
{
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

// alloc (first), free & sizeof
void *st_alloc(size_t size)
{
}
int st_free(void *ptr)
{
}
size_t st_sizeof(void *ptr)
{
  // Retrieving coresponding Node
  Node *n = st_ptrToNode(ptr);

  // Distance to next node - size of Node
  return n->next - st_indexOf(n) - sizeof(Node);
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
