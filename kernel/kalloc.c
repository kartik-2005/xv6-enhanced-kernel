// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;


// CHANGED CODE
// Add this near the other spinlock definitions
struct spinlock ref_lock;
// This array will store the reference count for each physical page
uint ref_counts[(PHYSTOP - KERNBASE) / PGSIZE];

// Helper function prototypes to be used in other files
void inc_ref(uint64 pa);
void dec_ref(uint64 pa);
// CHANGED CODE END


void
kinit()
{
  initlock(&kmem.lock, "kmem");
  
  // CHANGED
  initlock(&ref_lock, "ref_count"); // Add this line
  // END
  
  freerange(end, (void*)PHYSTOP);
}


// CHANGED
// A helper function to convert a physical address to an index in our ref_counts array
uint
pa_to_ref_idx(uint64 pa)
{
  return (pa - KERNBASE) / PGSIZE;
}

// Increment the reference count for a given physical page
void
inc_ref(uint64 pa)
{
  acquire(&ref_lock);
  ref_counts[pa_to_ref_idx(pa)]++;
  release(&ref_lock);
}

// Decrement the reference count for a given physical page
void
dec_ref(uint64 pa)
{
  acquire(&ref_lock);
  ref_counts[pa_to_ref_idx(pa)]--;
  release(&ref_lock);
}
// END


// void
// freerange(void *pa_start, void *pa_end)
// {
//   char *p;
//   p = (char*)PGROUNDUP((uint64)pa_start);
//   for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
//     kfree(p);
// }


// CHANGED
// Add memory to the free list.
void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE) {
    // New part: Before freeing the page for the first time,
    // set its reference count to 1. This prepares it
    // for our modified kfree().
    acquire(&ref_lock);
    ref_counts[pa_to_ref_idx((uint64)p)] = 1;
    release(&ref_lock);

    // Now call kfree, which will decrement the count to 0
    // and add the page to the freelist.
    kfree(p);
  }
}
// END


// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
// void
// kfree(void *pa)
// {
//   struct run *r;

//   if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
//     panic("kfree");

//   // Fill with junk to catch dangling refs.
//   memset(pa, 1, PGSIZE);

//   r = (struct run*)pa;

//   acquire(&kmem.lock);
//   r->next = kmem.freelist;
//   kmem.freelist = r;
//   release(&kmem.lock);
// }

// CHANGED
void
kfree(void *pa)
{
  struct run *r;
  uint ref_idx = pa_to_ref_idx((uint64)pa);
  int should_free = 0;

  acquire(&ref_lock);
  if(ref_counts[ref_idx] <= 0)
    panic("kfree: ref count <= 0");

  ref_counts[ref_idx]--;
  if(ref_counts[ref_idx] == 0) {
    should_free = 1;
  }
  release(&ref_lock);

  if(should_free) {
    memset(pa, 1, PGSIZE);
    r = (struct run*)pa;
    acquire(&kmem.lock);
    r->next = kmem.freelist;
    kmem.freelist = r;
    release(&kmem.lock);
  }
}
// END


// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
// void *
// kalloc(void)
// {
//   struct run *r;

//   acquire(&kmem.lock);
//   r = kmem.freelist;
//   if(r)
//     kmem.freelist = r->next;
//   release(&kmem.lock);

//   if(r)
//     memset((char*)r, 5, PGSIZE); // fill with junk
//   return (void*)r;
// }


// CHANGED
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r) {
    kmem.freelist = r->next;
    release(&kmem.lock);

    // Initialize reference count to 1 after acquiring
    acquire(&ref_lock);
    ref_counts[pa_to_ref_idx((uint64)r)] = 1;
    release(&ref_lock);
  } else {
    release(&kmem.lock);
  }

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}

// END