#pragma once


void *trimalloc(size_t size);
void  trifree(void  *memptr);

// A type used to allocate memory.  firstblock is the first block of items.
// nowblock is the block from which items are currently being allocated.
// nextitem points to the next slab of free memory for an item.
// deaditemstack is the head of a linked list (stack) of deallocated items
// that can be recycled.  unallocateditems is the number of items that
// remain to be allocated from nowblock.
//
// Traversal is the process of walking through the entire list of items, and
// is separate from allocation.  Note that a traversal will visit items on
// the "deaditemstack" stack as well as live items.  pathblock points to
// the block currently being traversed.  pathitem points to the next item
// to be traversed.  pathitemsleft is the number of items that remain to
// be traversed in pathblock.
//
// alignbytes determines how new records should be aligned in memory.
// getItemBytes() is the length of a record in bytes (after rounding up).
// itemsperblock is the number of items allocated at once in a single
// block.  itemsfirstblock is the number of items in the first block,
// which can vary from the others.  items is the number of currently
// allocated items.  maxitems is the maximum number of items that have
// been allocated at once; it is the current number of items plus the
// number of records kept on deaditemstack.
template<class T> class MemoryPool {
private:
  void **firstblock, **nowblock;
  void  *nextitem;
  void  *deaditemstack;
  void **pathblock;
  void  *pathitem;
  int    alignbytes;
  int    itembytes;
  int    itemsperblock;
  int    itemsfirstblock;
  long   items, maxitems;
  int    unallocateditems;
  int    pathitemsleft;
  void   poolZero();
public:
  MemoryPool() {
    poolZero();
  }
  void restart();
  void poolinit(int bytecount, int itemcount, int firstitemcount, int alignment);
  void pooldeinit();
  T *alloc();
  void dealloc(T *dyingitem);
  void traversalinit();
  T *traverse();

  void resetDeadItems() {
    deaditemstack = NULL;
  }
  inline int getAlignBytes() const {
    return alignbytes;
  }
  inline int getItemBytes() const {
    return itembytes;
  }
  inline long getItems() const {
    return items;
  }
  inline long getMaxItems() const {
    return maxitems;
  }
  inline void **getFirstBlock() {
    return firstblock;
  }
  inline int getItemsFirstBlock() const {
    return itemsfirstblock;
  }
  inline int getItemsPerBlock() const {
    return itemsperblock;
  }
};

//***************************************************************************
//  poolZero()   Set all of a pool's fields to zero.
//  This procedure should never be called on a pool that has any memory
//  allocated to it, as that memory would leak.
//***************************************************************************
template<class T> void MemoryPool<T>::poolZero() {
  firstblock       = NULL;
  nowblock         = NULL;
  nextitem         = NULL;
  deaditemstack    = NULL;
  pathblock        = NULL;
  pathitem         = NULL;
  alignbytes       = 0;
  itembytes        = 0;
  itemsperblock    = 0;
  itemsfirstblock  = 0;
  items            = 0;
  maxitems         = 0;
  unallocateditems = 0;
  pathitemsleft    = 0;
}

//***************************************************************************
//  restart()   Deallocate all items in a pool.
//  The pool is returned to its starting state, except that no memory is
//  freed to the operating system.  Rather, the previously allocated blocks
//  are ready to be reused.
//***************************************************************************
template<class T> void MemoryPool<T>::restart() {
  size_t alignptr;

  items    = 0;
  maxitems = 0;

  // Set the currently active block.
  nowblock = firstblock;
  // Find the first item in the pool.  Increment by the size of (void *).
  alignptr = (size_t)(nowblock + 1);
  // Align the item on an `alignbytes'-byte boundary.
  nextitem = (void*)(alignptr + (unsigned long) alignbytes - (alignptr % (unsigned long) alignbytes));
  // There are lots of unallocated items left in this block.
  unallocateditems = itemsfirstblock;
  // The stack of deallocated items is empty.
  resetDeadItems();
}

//***************************************************************************
//  poolinit()   Initialize a pool of memory for allocation of items.
//  This routine initializes the machinery for allocating items.  A `pool'
//  is created whose records have size at least `bytecount'.  Items will be
//  allocated in `itemcount'-item blocks.  Each item is assumed to be a
//  collection of words, and either pointers or floating-point values are
//  assumed to be the "primary" word type.  (The "primary" word type is used
//  to determine alignment of items.)  If `alignment' isn't zero, all items
//  will be `alignment'-byte aligned in memory.  `alignment' must be either
//  a multiple or a factor of the primary word size; powers of two are safe.
//  `alignment' is normally used to create a few unused bits at the bottom
//  of each item's pointer, in which information may be stored.
//
//  Don't change this routine unless you understand it.
//***************************************************************************
template<class T> void MemoryPool<T>::poolinit(int bytecount, int itemcount, int firstitemcount, int alignment) {
  // Find the proper alignment, which must be at least as large as:
  //   - The parameter `alignment'.
  //   - sizeof(void*), so the stack of dead items can be maintained
  //       without unaligned accesses.
  if (alignment > sizeof(void*)) {
    alignbytes = alignment;
  } else {
    alignbytes = sizeof(void*);
  }
  itembytes = ((bytecount - 1) / alignbytes + 1) * alignbytes;
  itemsperblock = itemcount;
  if (firstitemcount == 0) {
    itemsfirstblock = itemcount;
  } else {
    itemsfirstblock = firstitemcount;
  }

  // Allocate a block of items.  Space for `itemsfirstblock' items and one
  // pointer (to point to the next block) are allocated, as well as space
  // to ensure alignment of the items.
  firstblock = (void**)trimalloc(itemsfirstblock * getItemBytes() + sizeof(void*) + alignbytes);
  // Set the next block pointer to NULL.
  *(firstblock) = NULL;
  restart();
}

// Free to the operating system all memory taken by a pool.
template<class T> void MemoryPool<T>::pooldeinit() {
  while (firstblock != NULL) {
    nowblock = (void**) *(firstblock);
    trifree((void*)firstblock);
    firstblock = nowblock;
  }
}

// Allocate space for an item.
template<class T> T *MemoryPool<T>::alloc() {
  void  *newitem;
  void **newblock;
  size_t alignptr;

  // First check the linked list of dead items.  If the list is not
  // empty, allocate an item from the list rather than a fresh one.
  if (deaditemstack != NULL) {
    newitem = deaditemstack;               // Take first item in list.
    deaditemstack = *(void**)deaditemstack;
  } else {
    // Check if there are any free items left in the current block.
    if (unallocateditems == 0) {
      // Check if another block must be allocated.
      if (*(nowblock) == NULL) {
        // Allocate a new block of items, pointed to by the previous block.
        newblock = (void**)trimalloc(itemsperblock * getItemBytes() + sizeof(void*) + alignbytes);
        *(nowblock) = (void*)newblock;
        // The next block pointer is NULL.
        *newblock = NULL;
      }

      // Move to the new block.
      nowblock = (void**)*(nowblock);
      // Find the first item in the block.
      // Increment by the size of (void *).
      alignptr = (size_t)(nowblock + 1);
      // Align the item on an `alignbytes'-byte boundary.
      nextitem = (void*)(alignptr + (unsigned long) alignbytes
                      - (alignptr % (unsigned long) alignbytes));
      // There are lots of unallocated items left in this block.
      unallocateditems = itemsperblock;
    }

    // Allocate a new item.
    newitem = nextitem;
    // Advance `nextitem' pointer to next free item in block.
    nextitem = (void*)((char*)nextitem + getItemBytes());
    unallocateditems--;
    maxitems++;
  }
  items++;
  return (T*)newitem;
}

//  dealloc()   Deallocate space for an item
//  The deallocated space is stored in a queue for later reuse
template<class T> void MemoryPool<T>::dealloc(T *dyingitem) {
  // Push freshly killed item onto stack.
  *((void**)dyingitem) = deaditemstack;
  deaditemstack = dyingitem;
  items--;
}

//  traversalinit()   Prepare to traverse the entire list of items
//  This routine is used in conjunction with traverse()
template<class T> void MemoryPool<T>::traversalinit() {
  size_t alignptr;

  // Begin the traversal in the first block.
  pathblock = firstblock;
  // Find the first item in the block.  Increment by the size of (void*).
  alignptr = (size_t)(pathblock + 1);
  // Align with item on an `alignbytes'-byte boundary.
  pathitem = (void*)(alignptr + (unsigned long) alignbytes
                  - (alignptr % (unsigned long) alignbytes));
  // Set the number of items left in the current block.
  pathitemsleft = itemsfirstblock;
}

//***************************************************************************
//
//  traverse()   Find the next item in the list.
//
//  This routine is used in conjunction with traversalinit().  Be forewarned
//  that this routine successively returns all items in the list, including
//  deallocated ones on the deaditemqueue.  It's up to you to figure out
//  which ones are actually dead.  Why?  I don't want to allocate extra
//  space just to demarcate dead items.  It can usually be done more
//  space-efficiently by a routine that knows something about the structure
//  of the item.
//
//***************************************************************************
template<class T> T *MemoryPool<T>::traverse() {
  void  *newitem;
  size_t alignptr;

  // Stop upon exhausting the list of items.
  if (pathitem == nextitem) {
    return NULL;
  }

  // Check whether any untraversed items remain in the current block.
  if (pathitemsleft == 0) {
    // Find the next block.
    pathblock = (void**)*(pathblock);
    // Find the first item in the block.  Increment by the size of (void*).
    alignptr = (size_t)(pathblock + 1);
    // Align with item on an `alignbytes'-byte boundary.
    pathitem = (void*)(alignptr + (unsigned long) alignbytes
             -        (alignptr % (unsigned long) alignbytes));
    // Set the number of items left in the current block.
    pathitemsleft = itemsperblock;
  }

  newitem = pathitem;
  // Find the next item in the block.
  pathitem = (void*)((char*)pathitem + getItemBytes());
  pathitemsleft--;
  return (T*)newitem;
}
