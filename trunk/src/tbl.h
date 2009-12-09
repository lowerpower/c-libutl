/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This sofwtare is distributed under the terms of the BSD license:
**   http://creativecommons.org/licenses/BSD/
**   http://opensource.org/licenses/bsd-license.php 
*/


#ifndef TBL_H
#define TBL_H

/*
.v
             _    _      _
            / \  / \    / \
         __/  (_/  /   /  /
        (_   ___) (_  /  /
         /  / /  __ \/  /
        /  /_/  /_/ /  /
       (_____)_____(__/
..
*/


/***** libutl/Table
 * DESCRIPTION 
 * Generic Tables 
******
*/ 
/* VAL *****************/


/****t* Table/Values
* DESCRIPTION
*  Table Value. 
*  The union that represents a value that can be used in a tbl_h table
* 
* BUGS 
*   Assumes that it is safe to write in p a void * and read it
*   back as a char * from s. 
*   ACCORDING TO ANSI C STANDARD THIS COULD CAUSE
*   AN UNDEFINED BEHAVIOUR.
* SOURCE
*/

typedef union {
  void  *p;    /* generic pointer */
  char  *s;    /* string */
  long   n;    /* integer number */
  float  f;    /* floating point number */
} val_u;

/********/

#define valGetN(v) ((v).n)
#define valGetS(v) ((v).s)
#define valGetP(v) ((v).p)
#define valGetT(v) ((tbl_t)((v).p))
#define valGetV(v) ((vec_t)((v).p))
#define valGetO(v) ((v).p)

/* TBL *****************/
/**
* A table slot.
*/ 
typedef struct {
  unsigned char flg[4];  /* flags are used to store the type of key/value*/
  val_u  val;  /*  stored value */
  val_u  key;  /*  slot key */
  val_u  aux;  /* The aux value will be used for different purposes */
} tbl_slot_t;    

/**
* The table itself.
* This structure can be reallocated due to insertion and deletion. 
*/

typedef struct {
  long count;  /**< number of used slots */
  long size;   /**< number of allocated slots */
  long cur;    /**< "current" slot used for iterations*/
  tbl_slot_t slot[1];  /**< start of the slot's array */
} tbl_ht_t;

/**
*  The type to use to define a table.
*/ 

typedef tbl_ht_t *tbl_t;

tbl_t tbl_new(long nslots);

#define tblNew(t) (t=tbl_new(4))

val_u tbl_get(tbl_t tb, char tk, long nkey, void *pkey,
                        char tv, long ndef, void *pdef);

#define tblGetNN(tb,k,d)  valGetN(tbl_get(tb,'N', k, NULL, 'N', d, NULL))
#define tblGetSN(tb,k,d)  valGetN(tbl_get(tb,'S', 0, k   , 'N', d, NULL))
#define tblGetPN(tb,k,d)  valGetN(tbl_get(tb,'P', 0, k   , 'N', d, NULL))
#define tblGetTN(tb,k,d)  valGetN(tbl_get(tb,'T', 0, k   , 'N', d, NULL))
#define tblGetON(tb,k,d)  valGetN(tbl_get(tb,'O', 0, k   , 'N', d, NULL))

#define tblGetNS(tb,k,d)  valGetS(tbl_get(tb, 'N', k, NULL, 'S', 0, d))
#define tblGetSS(tb,k,d)  valGetS(tbl_get(tb, 'S', 0, k   , 'S', 0, d))
#define tblGetPS(tb,k,d)  valGetS(tbl_get(tb, 'P', 0, k   , 'S', 0, d))
#define tblGetTS(tb,k,d)  valGetS(tbl_get(tb, 'T', 0, k   , 'S', 0, d))
#define tblGetOS(tb,k,d)  valGetS(tbl_get(tb, 'O', 0, k   , 'S', 0, d))

#define tblGetNP(tb,k,d)  valGetP(tbl_get(tb, 'N', k, NULL, 'P', 0, d))
#define tblGetSP(tb,k,d)  valGetP(tbl_get(tb, 'S', 0, k   , 'P', 0, d))
#define tblGetPP(tb,k,d)  valGetP(tbl_get(tb, 'P', 0, k   , 'P', 0, d))
#define tblGetTP(tb,k,d)  valGetP(tbl_get(tb, 'T', 0, k   , 'P', 0, d))
#define tblGetOP(tb,k,d)  valGetP(tbl_get(tb, 'O', 0, k   , 'P', 0, d))

#define tblGetNT(tb,k,d)  valGetT(tbl_get(tb, 'N', k, NULL, 'T', 0, d))
#define tblGetST(tb,k,d)  valGetT(tbl_get(tb, 'S', 0, k   , 'T', 0, d))
#define tblGetPT(tb,k,d)  valGetT(tbl_get(tb, 'P', 0, k   , 'T', 0, d))
#define tblGetTT(tb,k,d)  valGetT(tbl_get(tb, 'T', 0, k   , 'T', 0, d))
#define tblGetOT(tb,k,d)  valGetT(tbl_get(tb, 'P', 0, k   , 'T', 0, d))

#define tblGetNO(tb,k,d)  valGetP(tbl_get(tb, 'N', k, NULL, 'O', 0, d))
#define tblGetSO(tb,k,d)  valGetP(tbl_get(tb, 'S', 0, k   , 'O', 0, d))
#define tblGetPO(tb,k,d)  valGetP(tbl_get(tb, 'P', 0, k   , 'O', 0, d))
#define tblGetTO(tb,k,d)  valGetP(tbl_get(tb, 'T', 0, k   , 'O', 0, d))
#define tblGetOO(tb,k,d)  valGetP(tbl_get(tb, 'O', 0, k   , 'O', 0, d))

long    tblNext (tbl_t tb, long start);
#define tblFirst(tb,i) tblNext(tb,0)

#define tblForeach(tb,i) for(i=tblNext(tb,0); (long)i >= 0; \
                                                     i = tblNext(tb,(long)i))

#define tblNil 0
        
#define tbl_keytype(slot)  ((slot)->flg[0])
#define tbl_valtype(slot)  ((slot)->flg[1])

#define tblSlot(tb,i)  ((tb)->slot[(i)-1])

#define tblCount(tb) ((tb)? (tb)->count : 0)

extern long tbl_i;

#define tblKeyType(tb,i) ((tbl_i=(i))? tbl_keytype(&tblSlot(tb,tbl_i)) : '\0') 
#define tblValType(tb,i) ((tbl_i=(i))? tbl_valtype(&tblSlot(tb,tbl_i)) : '\0')
                                     
#define tblKeyN(tb,i) valGetN(tblSlot(tb,i).key) 
#define tblKeyS(tb,i) valGetS(tblSlot(tb,i).key) 
#define tblKeyP(tb,i) valGetP(tblSlot(tb,i).key) 
#define tblKeyT(tb,i) valGetT(tblSlot(tb,i).key)
#define tblKeyO(tb,i) valGetO(tblSlot(tb,i).key) 
 
#define tblValN(tb,i) valGetN(tblSlot(tb,i).val) 
#define tblValS(tb,i) valGetS(tblSlot(tb,i).val) 
#define tblValP(tb,i) valGetP(tblSlot(tb,i).val) 
#define tblValT(tb,i) valGetT(tblSlot(tb,i).val) 
#define tblValO(tb,i) valGetO(tblSlot(tb,i).val) 

#define tblDel(tb,i) (tbl_keytype(&tblSlot(tb,i)) = '\0')

long tbl_find(tbl_t tb, char tk, long nkey, void *pkey);

#define tblFindN(tb,k) tbl_find(tb, 'N', k, NULL)
#define tblFindS(tb,k) tbl_find(tb, 'S', 0, k)
#define tblFindP(tb,k) tbl_find(tb, 'P', 0, k)
#define tblFindT(tb,k) tbl_find(tb, 'T', 0, k)
#define tblFindO(tb,k) tbl_find(tb, 'O', 0, k)

tbl_t tbl_set(tbl_t tb, char tk, long nkey, void *pkey,
                        char tv, long nval, void *pval);

#define tbl_Set(t,tk,nk,pk,tv,nv,pv) (t = tbl_set(t,tk,nk,pk,tv,nv,pv))

#define tblSetNN(tb,k,v)  tbl_Set(tb, 'N', k, NULL,      'N', v, NULL)
#define tblSetSN(tb,k,v)  tbl_Set(tb, 'S', 0, chsDup(k), 'N', v, NULL)
#define tblSetPN(tb,k,v)  tbl_Set(tb, 'P', 0, k,         'N', v, NULL)
#define tblSetTN(tb,k,v)  tbl_Set(tb, 'T', 0, k,         'N', v, NULL)
#define tblSetON(tb,k,v)  tbl_Set(tb, 'O', 0, k,         'N', v, NULL)
                                   
#define tblSetNS(tb,k,v)  tbl_Set(tb, 'N', k, NULL,      'S', 0, chsDup(v))
#define tblSetSS(tb,k,v)  tbl_Set(tb, 'S', 0, chsDup(k), 'S', 0, chsDup(v))
#define tblSetPS(tb,k,v)  tbl_Set(tb, 'P', 0, k,         'S', 0, chsDup(v))
#define tblSetTS(tb,k,v)  tbl_Set(tb, 'T', 0, k,         'S', 0, chsDup(v))
#define tblSetOS(tb,k,v)  tbl_Set(tb, 'O', 0, k,         'S', 0, chsDup(v))

#define tblSetNT(tb,k,v)  tbl_Set(tb, 'N', k, NULL,      'T', 0, v)
#define tblSetST(tb,k,v)  tbl_Set(tb, 'S', 0, chsDup(k), 'T', 0, v)
#define tblSetPT(tb,k,v)  tbl_Set(tb, 'P', 0, k   ,      'T', 0, v)
#define tblSetTT(tb,k,v)  tbl_Set(tb, 'T', 0, k   ,      'T', 0, v)
#define tblSetOT(tb,k,v)  tbl_Set(tb, 'O', 0, k   ,      'T', 0, v)

#define tblSetNP(tb,k,v)  tbl_Set(tb, 'N', k, NULL,      'P', 0, v)
#define tblSetSP(tb,k,v)  tbl_Set(tb, 'S', 0, chsDup(k), 'P', 0, v)
#define tblSetPP(tb,k,v)  tbl_Set(tb, 'P', 0, k,         'P', 0, v)
#define tblSetTP(tb,k,v)  tbl_Set(tb, 'T', 0, k,         'P', 0, v)
#define tblSetOP(tb,k,v)  tbl_Set(tb, 'O', 0, k,         'P', 0, v)

#define tblSetNO(tb,k,v)  tbl_Set(tb, 'N', k, NULL,      'O', 0, v)
#define tblSetSO(tb,k,v)  tbl_Set(tb, 'S', 0, chsDup(k), 'O', 0, v)
#define tblSetPO(tb,k,v)  tbl_Set(tb, 'P', 0, k,         'O', 0, v)
#define tblSetTO(tb,k,v)  tbl_Set(tb, 'T', 0, k,         'O', 0, v)
#define tblSetOO(tb,k,v)  tbl_Set(tb, 'O', 0, k,         'O', 0, v)

tbl_t tbl_free(tbl_t tb,char wipe);
#define tblFree(tb) (tb=tbl_free(tb,'w'))

tbl_t tbl_del(tbl_t tb, char tk, long nkey, void *pkey);

#define tbl_Del(t,tk,nk,pk) (t = tbl_del(t,tk,nk,pk))

#define tblDelN(tb,k) tbl_Del(tb, 'N', k, NULL)
#define tblDelS(tb,k) tbl_Del(tb, 'S', 0, k)
#define tblDelP(tb,k) tbl_Del(tb, 'P', 0, k)
#define tblDelT(tb,k) tbl_Del(tb, 'T', 0, k)
#define tblDelO(tb,k) tbl_Del(tb, 'O', 0, k)

tbl_t tblMaxSlot(tbl_t tb, long nslots);

/* VEC *****************/

typedef struct {
  unsigned char flg[4];
  val_u  val;
} vec_slot_t;

typedef struct {
  long count;
  long size;
  long cur;
  vec_slot_t slot[1];
} vec_st_t;

typedef vec_st_t *vec_t;

extern vec_t vec_tmp;
 
#define vec_valtype(slot)  ((slot)->flg[1])
#define vecValType(vt,i) \
 (vec_tmp=vt, \
 ((vec_tmp) && (vec_tmp->count < (i)))    ? \
         vec_valtype((vec_tmp)->slot+(i)) : \
         '\0')

val_u vec_get(vec_t tb, long nkey, char tv, long ndef, void *pdef);

vec_t vecSetCount(vec_t vt, long max);
long vecCount(vec_t vt);

extern vec_t vec_tmp;
#define vecVoid ((vec_t)utlEmptyString)

vec_t vec_new(long nslots);
#define vecNew(v) (v = vec_new(2))

vec_t vecDel(vec_t vt, long kfrom, long kto);
vec_t vecMove(vec_t vt, long kfrom, long kto);

vec_t vec_free(vec_t vt, char wipe);
#define vecFree(vt) (vt=vec_free(vt,1))

vec_t vec_set(vec_t tb, long nkey, char tv, long nval, void *pval);
#define vec_Set(t,n,tv,nv,pv) (t = vec_set(t,n,tv,nv,pv))

#define vecSetN(tb,k,v)  vec_Set(tb, k, 'N', v, NULL)
#define vecSetS(tb,k,v)  vec_Set(tb, k, 'S', 0, chsDup(v))
#define vecSetP(tb,k,v)  vec_Set(tb, k, 'P', 0, v)
#define vecSetT(tb,k,v)  vec_Set(tb, k, 'T', 0, v)
#define vecSetV(tb,k,v)  vec_Set(tb, k, 'V', 0, v)
#define vecSetO(tb,k,v)  vec_Set(tb, k, 'O', 0, v)

#define vecAddN(tb,v)    vec_Set(tb, vecCount(tb), 'N', v, NULL)
#define vecAddS(tb,v)    vec_Set(tb, vecCount(tb), 'S', 0, chsDup(v))
#define vecAddP(tb,v)    vec_Set(tb, vecCount(tb), 'P', 0, v)
#define vecAddT(tb,v)    vec_Set(tb, vecCount(tb), 'T', 0, v)
#define vecAddV(tb,v)    vec_Set(tb, vecCount(tb), 'V', 0, v)
#define vecAddO(tb,v)    vec_Set(tb, vecCount(tb), 'O', 0, v)

#define vecGetN(tb,k,d)  valGetN(vec_get(tb, k , 'N', d, NULL))
#define vecGetS(tb,k,d)  valGetS(vec_get(tb, k , 'S', 0, d))
#define vecGetP(tb,k,d)  valGetP(vec_get(tb, k , 'P', 0, d))
#define vecGetT(tb,k,d)  valGetT(vec_get(tb, k , 'T', 0, d))
#define vecGetV(tb,k,d)  valGetV(vec_get(tb, k , 'V', 0, d))
#define vecGetO(tb,k,d)  valGetP(vec_get(tb, k , 'O', 0, d))

#define vecGetRawN(tb,k)  valGetN(vec_get(tb, k , 'N', d, NULL))
#define vecGetRawS(tb,k)  valGetS(vec_get(tb, k , 'S', 0, d))
#define vecGetRawP(tb,k)  valGetP(vec_get(tb, k , 'P', 0, d))
#define vecGetRawT(tb,k)  valGetT(vec_get(tb, k , 'T', 0, d))
#define vecGetRawO(tb,k)  valGetP(vec_get(tb, k , 'O', 0, d))

vec_t vec_ins(vec_t tb, long nkey, char tv, long nval, void *pval);
#define vec_Ins(t,nk,tv,nv,pv) (t = vec_ins(t,nk,tv,nv,pv))

#define vecInsN(tb,k,v)  vec_Ins(tb,  k, 'N', v, NULL)
#define vecInsS(tb,k,v)  vec_Ins(tb,  k, 'S', 0, chsDup(v))
#define vecInsP(tb,k,v)  vec_Ins(tb,  k, 'P', 0, v)
#define vecInsT(tb,k,v)  vec_Ins(tb,  k, 'T', 0, v)
#define vecInsV(tb,k,v)  vec_Ins(tb,  k, 'V', 0, v)
#define vecInsO(tb,k,v)  vec_Ins(tb,  k, 'O', 0, v)

vec_t vec_del(vec_t tb, long kfrom, long kto);


/* .%% Other functions
   '''''''''''''''''''
*/
vec_t vec_split(char *s, char *sep,char *trim, int dup);

#define vecSplit(s, sep, trim) vec_split(s,sep,trim,1)
#define vecSplitP(s, sep, trim) vec_split(s,sep,trim,0)

#endif
