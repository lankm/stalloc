/* static memory
 * 
 * 
 * 
 */

#include <stdlib.h>
#include <stdio.h> //delete later

#define SYS_BYTE      sizeof(void*)
#define ALIGN(s)      ((s-1) / SYS_BYTE * SYS_BYTE) + SYS_BYTE

using namespace std;

class st_mem
{
  private:
    // Typedef Node
    typedef struct node {
    // Indexes
    size_t next;
    size_t prev : sizeof(size_t)*8-1; 

    size_t open : 1;
    } Node;

    // Allocated memory as byte array
    char *mem;

    static void * st_nodeToPtr(Node *n)
    {
      // Go forward by the size of node
      return (void*) &n[1];
    }
    static Node * st_ptrToNode(void *ptr)
    {
      // Go back by the size of node
      return (Node*) ptr-sizeof(Node);
    }
    
    size_t st_indexOf(void *ptr)
    {
      return (size_t) ( (char*)ptr - (char*)mem );
    }
    size_t st_sizeof(Node *n)
    {
      // Distance to next node - size of Node
      return n->next - st_indexOf(n) - sizeof(Node);
    }
    void st_free(Node *n)
    {

    }
  public:
    st_mem(size_t size)
    {
      // Alligning
      size = ALIGN(size);

      // Byte array init
      mem = (char*) malloc(size);

      // Indexes
      size_t head_i = 0;
      size_t tail_i = size-sizeof(Node);
      // Ptrs
      Node *head = (Node*) &mem[ head_i ];
      Node *tail = (Node*) &mem[ tail_i ];

      // Head Node setup
      head->next = tail_i;
      head->prev = head_i;
      head->open = 1;

      // Tail Node setup
      tail->next = tail_i;
      tail->prev = head_i;
      tail->open = 0;
    }
    ~st_mem()
    {
      free(mem);
    }

    void* st_malloc(size_t size)
    {
      return NULL;
    }
    void st_free(void *ptr)
    {

    }
    size_t st_sizeof(void *ptr)
    {
      // Retrieving coresponding Node
      Node *n = st_ptrToNode(ptr);

      return st_sizeof(n);
    }

    //delete later
    void st_printN(Node *n)
    {
      // Most usefull info
      printf("Node:<%p>| Size: %llu\n", n, st_sizeof(n));

      // Node variables
      printf("Next: %p |\n", &mem[ n->next ]);
      printf("Prev: %p | ", &mem[ n->prev ]);
      printf("Open: %d\n", n->open);
    }
    void st_print()
    {
      Node *n = (Node*) mem;
      do
      {
        // Print
        st_printN(n);
        printf("\n");

        // Go to next
        n = (Node*) &mem[ n->next ];

        // While next isn't itself
      } while ( n != (Node*)&mem[ n->next ] );

      // Print last node
      // last node underflows its size because it has none and is last
      st_printN(n);
    }
};
