/* static memory
 * 
 * malloc = 800
 * st_mem = 430
 * stalloc_v1 = 270
 * stalloc = 340
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

    // Node <-> Ptr
    static void * st_nodeToPtr(Node *n)
    {
      // Go forward by the size of node
      return (void*) &n[1];
    }
    static Node * st_ptrToNode(void *ptr)
    {
      // Go back by the size of node
      return (Node*) ( (char*)ptr - sizeof(Node) );
    }

    // Utility
    size_t st_indexOf(void *ptr)
    {
      return (size_t) ( (char*)ptr - (char*)mem );
    }
    void st_split(Node *split, Node *create)
    {
      // ptr to node after 'create'
      Node *next = (Node*) &mem[ split->next ];

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
      Node *next = (Node*) &mem[ remove->next ];

      // join <-> next
      join->next = remove->next;
      next->prev = remove->prev;
    }
    
    // Duplicates for nodes and not pointers
    void st_free(Node *n)
    {

    }
    size_t st_sizeof(Node *n)
    {
      // Distance to next node - size of Node
      return n->next - st_indexOf(n) - sizeof(Node);
    }
  
  public:
    // construct / destruct
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

    // core functions
    void* st_malloc(size_t size)
    {
      size = ALIGN(size);

      // Start from beginning
      Node *n = (Node*) mem;
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
            Node *create = (Node*) &mem[ create_i];
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
        n = (Node*) &mem[ n->next ];
      } while ( st_indexOf(n) != n->next);

      // No block found
      return NULL;
    }
    void st_free(void *ptr)
    {
      // get relevent nodes
      Node *n = st_ptrToNode(ptr), 
           *prev = (Node*) &mem[ n->prev ], 
           *next = (Node*) &mem[ n->next ];

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
      // Retrieving coresponding Node
      Node *n = st_ptrToNode(ptr);

      // return the size of it
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
