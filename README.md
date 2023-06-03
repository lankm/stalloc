# Stalloc

A Static memory allocator

Implments a byte array with block nodes placed before given pointers. Uses first_search for now but will change once I decide on complex implemention. Node implements pointers to other nodes.

Testing method: simple malloc, sizeof, then free 1,000,000,000 times, and use clock() before and after. This tests the overhead of stalloc. In depth testing for the new alloc method be done propperly. Optimized testing for malloc always results in zero because no 'actual' work is being done so compare the unoptimized execution of malloc to the execution of optimized stalloc because malloc itself is already optimized.

Unoptimized
- Malloc      (  37,974,891 Tics ) x
- stalloc.h   ( 127,191,651 Tics )
- stalloc.hpp ( 124,542,994 Tics ) 

Optimized (-O2)
- malloc      (  N/A  )
- stalloc.h   ( 21,229,656 Tics ) x
- stalloc.hpp ( 21,843,366 Tics ) x

As you can see, the overhead of stalloc is almost half of malloc when dealing with a small amount of pointers. The time complexity of stalloc is worse than malloc so if you have >50 pointers, use malloc instead. This was my little introduction to performance testing and should not be used over malloc in other projects. Look at the implementation though; its pretty nifty in my opinion.



