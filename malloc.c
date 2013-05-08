#define _GNU_SOURCE
#include "brk.h"
#include <unistd.h>
#include <string.h> 
#include <stdio.h>
#include <errno.h> 
#include <sys/mman.h>

#define NALLOC 1024                                     /* minimum #units to request */

typedef long Align;                                     /* for alignment to long boundary */

union header {                                          /* block header */
  struct {
    union header *ptr;                                  /* next block if on free list */
    unsigned size;                                      /* size of this block  - what unit? */ 
  } s;
  Align x;                                              /* force alignment of blocks */
};

typedef union header Header;

static Header base;                                     /* empty list to get started */
static Header *freep = NULL;                            /* start of free list */

/* free: put block ap in the free list */

void free(void * ap)
{
  Header *bp, *p;

  if(ap == NULL) return;                                /* Nothing to do */

  bp = (Header *) ap - 1;                               /* point to block header */
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;                                            /* freed block at atrt or end of arena */

  if(bp + bp->s.size == p->s.ptr) {                     /* join to upper nb */
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  }
  else
    bp->s.ptr = p->s.ptr;
  if(p + p->s.size == bp) {                             /* join to lower nbr */
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
}

/* morecore: ask system for more memory */

#ifdef MMAP

static void * __endHeap = 0;

void * endHeap(void)
{
  if(__endHeap == 0) __endHeap = sbrk(0);
  return __endHeap;
}
#endif


static Header *morecore(unsigned nu)
{
  void *cp;
  Header *up;
#ifdef MMAP
  unsigned noPages;
  if(__endHeap == 0) __endHeap = sbrk(0);
#endif

  if(nu < NALLOC)
    nu = NALLOC;
#ifdef MMAP
  noPages = ((nu*sizeof(Header))-1)/getpagesize() + 1;
  cp = mmap(__endHeap, noPages*getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  nu = (noPages*getpagesize())/sizeof(Header);
  __endHeap += noPages*getpagesize();
#else
  cp = sbrk(nu*sizeof(Header));
#endif
  if(cp == (void *) -1){                                 /* no space at all */
    perror("failed to get more memory");
    return NULL;
  }
  up = (Header *) cp;
  up->s.size = nu;
  free((void *)(up+1));
  return freep;
}

void * malloc(size_t nbytes)
{
  Header *p, *prevp;
  Header * morecore(unsigned);
  unsigned nunits;

  if(nbytes == 0) return NULL;


  nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) +1;

  if((prevp = freep) == NULL) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  #if STRATEGY==1
  for(p= prevp->s.ptr;  ; prevp = p, p = p->s.ptr) {
    if(p->s.size >= nunits) {                           /* big enough */
      if (p->s.size == nunits)                          /* exactly */
	prevp->s.ptr = p->s.ptr;
      else {                                            /* allocate tail end */
	p->s.size -= nunits;
	p += p->s.size;
	p->s.size = nunits;
      }
      freep = prevp;
      return (void *)(p+1);
    }
    if(p == freep)                                      /* wrapped around free list */
      if((p = morecore(nunits)) == NULL)
	return NULL;                                    /* none left */
  }
  #endif
  #if STRATEGY==2
  Header *bestSoFar = NULL;
  Header *bestPrev = NULL;
  for(p= prevp->s.ptr;  ; prevp = p, p = p->s.ptr) {
    if(p->s.size >= nunits) {                           /* big enough */
      if (p->s.size == nunits)                          /* exactly */
	  {
		prevp->s.ptr = p->s.ptr;
		freep = prevp;
		return (void*)(p+1);
	  }
      else {                                            /* allocate tail end */
	/*p->s.size -= nunits;
	p += p->s.size;
	p->s.size = nunits;*/
      }
	  if(bestSoFar == NULL || p->s.size < bestSoFar->s.size)
	  {
		bestSoFar = p;
		bestPrev = prevp;
	  }
		
	  /*
      freep = prevp;
      return (void *)(p+1);*/
    }
    if(p == freep)                          /* wrapped around free list */
	{
		if (bestSoFar != NULL)
		{
			bestSoFar->s.size -= nunits;
			bestSoFar += bestSoFar->s.size;
			bestSoFar->s.size = nunits;
			freep = bestPrev;
			return (void *)(bestSoFar+1);
		}
	    if((p = morecore(nunits)) == NULL)
			return NULL;                        /* NoNE LEFT*/
	}            
  }
  #endif
}

void * realloc(void *p, size_t size)
{
	if(p == NULL)
	{
		return malloc(size);
	}
	Header *h = (Header *) p - 1;
	size_t oldSize = h->s.size * sizeof(Header);
 	size_t nunits = (size+sizeof(Header)-1)/sizeof(Header) +1;
		void *np;
		np = malloc(size);
		/*size_t aligned = h->s.size;
		while( aligned % sizeof(Header) != 0)
		{
			aligned++;
		}
		fprintf(stderr, "%lu %lu %lu| ",size, h->s.size, aligned);*/
			memcpy(np, p, (size > oldSize -sizeof(Header))? oldSize-sizeof(Header): size);
		free(p);
		return np;
	/*
	Header *nh = (Header*) p + size;
	nh->s.size = h->s.size - nunits - 1; 
	h->s.size = nunits;
	nh->s.ptr = h->s.ptr;
	h->s.ptr = nh;
	return p;*/
}
