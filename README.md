# Stalloc

A Static memory allocator

Implments a byte array with block nodes placed before given pointers. Uses first_search. Node implement pointers to other nodes.

Testing method: simple malloc, sizeof, then free 1,000,000,000 times, And use clock() before and after.

Unoptimized
- Malloc      (  37,974,891 Tics )
- stalloc.h   ( 127,191,651 Tics )
- stalloc.hpp ( 124,542,994 Tics )

Optimized (-O2)
- malloc      (  N/A  )
- stalloc.h   ( 21,229,656 Tics )
- stalloc.hpp ( 21,843,366 Tics )





