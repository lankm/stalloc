# Stalloc

A Static memory allocator

Implments a byte array with block nodes placed before given pointers. Uses first_search. Node implement pointers to other nodes.

Testing method: simple malloc, sizeof, then free 1,000,000,000 times, And use clock() before and after.

Unoptimized
- Malloc      ( 41,114 )
- stalloc.h   ( 11,184 )
- stalloc.hpp ( 15,555 )

Optimized (-O2)
- malloc      (  N/A  )
- stalloc.h   ( 8,279 )
- stalloc.hpp ( 7,814 )



