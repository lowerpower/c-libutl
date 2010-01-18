/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the BSD license:
**   http://creativecommons.org/licenses/BSD/
**   http://opensource.org/licenses/bsd-license.php 
*/

#include <stdio.h>
#include <stdlib.h>
#include "tbl2.h"

/************************/

/*
** Integer log base 2 of a 32 bits integer values.
**   llog2(0) == llog2(1) == 0
*/
static unsigned short llog2(unsigned long x)
{
  long l = 0;

    if (x==0) return 0;
    #if defined(__POCC__) || defined(_MSC_VER) || defined (__WATCOMC__)
        /* Pelles C            MS Visual C++         OpenWatcom*/
      __asm { mov eax, [x]
              bsr ecx, eax
              mov  l, ecx
      }
      return (unsigned short)l;
    #elif defined(__GNUC__)
      return (unsigned short) __builtin_clzl(x);
    #elif
      if (x & 0xFFFF0000)         {l += 16; x >>= 16;}
      if (x & 0xFF00)             {l += 8;  x >>= 8 ;}
      if (x & 0xF0)               {l += 4;  x >>= 4 ;}
      if (x & 0xC)                {l += 2;  x >>= 2 ;}
      if (x & 2)                  {l += 1;           }
      return (unsigned short)l;
    #endif  
}

/******************************************************************/

/* from Bit Twiddling Hacks 
** http://www-graphics.stanford.edu/~seander/bithacks.html
*/ 

#define roundpow2(v) (v--,\
                     (v |= v >> 1), (v |= v >> 2), \
                     (v |= v >> 4), (v |= v >> 8), \
                     (v |= v >> 16), \
                      v++)

/******************************************************************/

#define val_set(x, tv,nv,pv,fv) ((tv == 'N') ? (void)((x).n = (nv)) : \
                                 (tv == 'F') ? (void)((x).f = (fv)) : \
                                               (void)((x).p = (pv))) 

#define val_del(tv,v) do { switch (tv) { \
                             case 'P' : (v).p = NULL; \
                             case 'N' : (v).n = 0; \
                             case 'U' : (v).u = 0; \
                             case 'F' : (v).f = 0.0; \
                           }\
                      } while (0)

static int val_cmp(char atype, val_u a, char btype, val_u b)
{
  int ret;
  
  ret = atype - btype;
  if (ret == 0) {
    switch (atype) {
      case '\0': ret = 0;                                          break;
      case 'P' : ret = (char *)(a.p) - (char *)(b.p);              break;
      case 'S' : ret = strcmp(a.s, b.s);                           break;
      case 'N' : ret = a.n - b.n;                                  break;
      case 'U' : ret = (a.u == b.u) ? 0 : ((a.u > b.u) ? 1 : -1);  break;
      case 'F' : ret = (a.f == b.f) ? 0 : ((a.f > b.f) ? 1 : -1);  break;
      
      default  : ret = -1;
    }
  }
  return ret;
}


/******************************************************************/

static void tbl_outofmem()
{ 
  if (stderr) fprintf(stderr,"OUT OF MEMORY");
  exit(1);
}


tbl_t tbl_new(long nslots)
{
  tbl_t tb = NULL;
  long sz;
  
  if (nslots & (nslots - 1)) roundpow2(nslots);
  if (nslots < 2) nslots = 2;
  sz = sizeof(tbl_table_t) + sizeof(tbl_slot_t) * (nslots-1);
  tb = calloc(1, sz);
  if (!tb) tbl_outofmem();
  
  tb->count = 0;
  tb->size  = nslots;
  return  tb;
}

#define isemptyslot(sl)    ((sl)->key_type == '\0')
#define setemptyslot(sl)   ((sl)->key_type = '\0')
#define tbl_slot(tb, k)    ((tb)->slot+(k))

#define TBL_SMALL 16
#define tbl_issmall(tb)    (tb->size <= TBL_SMALL)

static tbl_t tbl_compact(tbl_t tb)
{
  tbl_slot_t *slot_top;
  tbl_slot_t *slot_bot;
  
  slot_top = tb->slot;
  slot_bot = tb->slot + tb->size-1;
  
  while (slot_top < slot_bot) {
    while (slot_top < slot_bot &&  isemptyslot(slot_top))
      slot_top++;
    while (slot_top < slot_bot && !isemptyslot(slot_bot))
      slot_bot--;
    if (slot_top < slot_bot) {
      *slot_top = *slot_bot;
      setemptyslot(slot_bot);
      slot_top++;                    
      slot_bot--;                    
    }            
  }
}

tbl_t tbl_rehash(tbl_t tb, long nslots)
{
  tbl_t newtb;
  tbl_slot_t *slot;
  long ndx;
  
  nslots = roundpow2(nslots);
  if (tb->count > nslots) return tb;
  
  if (nslots <= TBL_SMALL) { /* It will be a small-table */
  
     /* If it was a hash table compact slots in the upper side */
    if (tb->size > TBL_SMALL) tb = tbl_compact(tb);
      
    /* Now only the first tbl->count slots are filled (and they are less than nslots)*/
    tb = realloc(tb, nslots);
    if (!tb) tbl_outofmem();
    
    if (nslots > tb->size) /* clear the newly added elemets */
      memset(tb->slot + tb->size, 0x00, (nslots-tb->size) * sizeof(tbl_slot_t));
      
    tb->size = nslots;
  }
  else { /* create a new table and fill it with existing elements */
    newtb = tbl_new(nslots);
    slot = tb->slot;
    for (ndx = 0; ndx < tb->size; ndx++, slot++) {
      if (!isemptyslot(slot))
        newtb = tbl_set(newtb, slot->key_type, slot->key,
                               slot->val_type, slot->val);
    }
    free(tb);
    tb = newtb;
  }
  return tb;
}


#define maxdist(tb) llog2(tb->size)

#define FIND_CANDIDATE  (-1)
#define FIND_EMPTY      (-2)
#define FIND_FULLTABLE  (-3)

static long tbl_find_small(tbl_t tb, char k_type, val_u key, long *candidate)
{
  tbl_slot_t *slot;
  
  for (slot = tb->slot; slot < tb->slot + tb->count; slot++) {
    if (val_cmp(k_type, key, slot->key_type, slot->key))
      return slot - tb->slot;
  }
  if (tb->count < tb->size) {
    *candidate = tb->count;
    return FIND_EMPTY;    
  }      
  return FIND_FULLTABLE;
}

static long tbl_find_hash(tbl_t tb, char k_type, val_u key, long *candidate, unsigned char *distance)
{
  long hk;  
  long h;
  long ndx;
  long d;
  long d_max;
  unsigned char cand_dist;
  tbl_slot_t *slot;
  
  d_max = maxdist(tb);
  
  *distance = d_max-1;
    
  hk = key_hash(k_type, key) % tb->size;
  ndx = hk;
  d = 0;
  
  while (d < d_max) {
    slot = tbl_slot(tb, ndx);
    
    if (isemptyslot(slot)) { 
      *distance = d;
      *candidate = ndx;
      return FIND_EMPTY;
    }
    
    if (ndx + slot->dist == hk) { /* same hash!! */
      if (val_cmp(k_type, key, slot->key_type, slot->key)) {
        *distance = d;
        *candidate = ndx;
        return ndx;
      }
    }
    else if (slot->dist <= *distance) {
      /* TODO: What's the best criteria for selecting candidate?
      **       Currently, the latest slot with lowest distance is selected.
      */
      *candidate = ndx;
      *distance = slot->dist;
    }
    
    ndx = (ndx + 1) % tb->size;
    d++;
  }
  if (*candidate >= 0) return FIND_CANDIDATE;
  return FIND_FULLTABLE; 
}

static long tbl_find(tbl_t tb, char k_type, val_u key, long *candidate, unsigned char *distance)
{
   *candidate = -1;
   *distance = 0;
         
   if (!tb)  return -1;
   
   if (tb->size <= TBL_SMALL)
     return tbl_find_small(tb, k_type, key, candidate);
     
   return tbl_find_hash(tb, k_type, key, candidate, distance);
}


val_u tbl_get(tbl_t tb, char k_type, val_u key, char v_type, val_u def)
{
  tbl_slot_t *slot;
  long ndx;
  long cand;
  unsigned char dist;
  
  ndx = tbl_find(tb, k_type, key, &cand, &dist);
  
  if (ndx < 0) return def;
  if (tb->slot[ndx].val_type != v_type)  return def;
  
  return (tb->slot[ndx].val);    
}




#define setslotkey(sl, k_t, k) \
  (((sl)->key_type = (k_t)), ((sl)->key = (k)))

#define setslotval(sl, v_t, v) \
 (((sl)->val_type = (v_t)), ((sl)->val = (v)))

#define setslotdist(sl, d) ((sl)->dist = (d))

static tbl_t tbl_add(tbl_t tb, char k_type, val_u key, char v_type, val_u val)
{

}

tbl_t tbl_set(tbl_t tb, char k_type, val_u key, char v_type, val_u val)
{
  long ndx;
  long cand = -1;
  unsigned char dist = 0;

  
  if (tb == NULL) {
    tb = tbl_new(2);
    tb->slot[0].key_type = k_type;
    tb->slot[0].key = key;
    tb->slot[0].val_type = v_type;
    tb->slot[0].val = val;
    tb->slot[0].dist = 0;
    tb->count++;
    return tb;
  }

  do {
    ndx = tbl_find(tb, k_type, key, &cand, &dist);
    
    if (ndx >= 0) {
      val_del(k_type, key);
      val_del(tbl->slot[ndx].val_type, tbl->slot[ndx].val);
      tb->slot[ndx].val_type = v_type;
      tb->slot[ndx].val = val;
      return tb;
    }
    
    if (ndx == FIND_CANDIDATE) {
      ndx = 
    }
    
    if (ndx == FIND_EMPTY) {
      tb->slot[cand].key_type = k_type;
      tb->slot[cand].key      = key;
      tb->slot[cand].val_type = v_type;
      tb->slot[cand].val      = val;
      tb->slot[cand].dist     = dist;
      tb->count++;
      return tb;
    }
    
    if (ndx == FIND_FULLTABLE) {
      tb = tbl_rehash(tb);
    }
  } while (ndx < 0);
  
  return tb;
}

#if 0
tbl_t tbl_del_small(tbl_t tb, char k_type, val_u key) 
{ 
  tbl_slot_t slot;
  long ndx;
  
  ndx = tbl_find_small(tb,k_type,key);
  if (ndx >= 0) {
    slot = tb->slot[ndx];
    val_del(slot->key_type, slot->key);
    val_del(slot->val_type, slot->val);
    tb->count--;
    if (ndx != tb->count) 
      tb->slot[ndx] = tb->slot[tb->count];
    tb->slot[tb->count].key_type = 0;
  }
   
  return tb;
}


tbl_t tbl_del(tbl_t tb, char tk, val_u key)
{



}
#endif

tbl_t tbl_free(tbl_t tb)
{
  tbl_slot_t *slot;
  long ndx;

  if (tb) {
    for (slot = tb->slot; slot < tb->slot + tb->size; slot++) {
      val_del(slot->key_type, slot->key);
      val_del(slot->val_type, slot->val);
    }
    free(tb);
  }
  return NULL;
}

int main()
{
  int k;
  tbl_t tb;
  
  tblNew(tb);
  tblFree(tb);   
  return 0; 
}
