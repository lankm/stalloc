# Stalloc

A Static memory allocator

Implments a byte array with block nodes placed before given pointers. Uses first_search
- Tested malloc                     (858)
- Tested Nodes with pointers        (270)
- Tested Nodes with indexes         (344)
- Tested Nodes with indexes in C++  (432)

Nodes with pointers won!! Sadly you can't create void* bitfields so theres a lot of casting
- Further reduced NwP down to 189.
- I'm not going to go crazy on the optimizations though.
- c++ equivelent runs at about a 10% hit to performance

Ok so ive come across something I 'like' and absolutely hate. stalloc_abomination turns all the void functions into #defines and gives a 15% boost to performance. For the sake of my sanity and the mercy of my comments, That file is being put in timeout. I shall think about whether it should live or die a quick death.

