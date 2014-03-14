/* TEST LINE 1
   TEST LINE (a rather long like, to tell the truth) 2
   TEST LINE 3
*/
/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This sofwtare is distributed under the terms of the BSD license:
**   http://creativecommons.org/licenses/BSD/
**   http://opensource.org/licenses/bsd-license.php 
*/

#define UTL_C
#define UTL_UNITTEST
#define UTL_MEMCHECK
#include "utl.h"

FILE *f = NULL;
char buf[512];
int k=0;
int c=0;
buf_t s=NULL;

int main (int argc, char *argv[])
{
  logLevel(logStderr,"DBG");
  logPre(logStderr,"#");
  
  TSTPLAN("utl unit test: buf") {
  
    TSTSECTION("buf creation") {
      TSTGROUP("bufNew()") {
        s = bufNew();
        TSTNEQPTR("Is not NULL", NULL, s );
        TSTEQINT("Mem Valid", utlMemValid, utlMemCheck(s));
      }
      TSTGROUP("Check on empty") {
        TSTEQINT("Len 0", 0, bufLen(s) );
        TSTEQINT("Max 0", 0, bufMax(s) );
        TSTEQPTR("NULL str", NULL, bufStr(s) );
        TSTEQINT("Get out out bound", '\0', bufGet(s,31));
      }
    }
    TSTSECTION("buf add") {
      TSTGROUP("buf set()") {
        bufSet(s,0,'a');
        TSTEQINT("Mem Valid", utlMemValid, utlMemCheck(bufStr(s)));
        TSTEQINT("Direct access", 'a', bufStr(s)[0] );
        TSTEQINT("Get (chrAt)", 'a', bufGet(s,0) );
        TSTEQINT("Len 1", 1, bufLen(s) );

        bufSet(s,1,'b');
        TSTEQINT("Len 2", 2, bufLen(s));
        TSTEQINT("Len 2", strlen(bufStr(s)), bufLen(s));
        TSTEQINT("Direct access", 0, strcmp("ab",bufStr(s)) );
        TSTFAILNOTE("str: [%s]\n",bufStr(s));
      }
      TSTGROUP("buf add char") {
        bufAdd(s,'c');
        TSTEQINT("Len 3", 3, bufLen(s));
        TSTEQINT("Len 3", strlen(bufStr(s)), bufLen(s));
        TSTEQINT("Set properly direct access", 0, strcmp("abc",bufStr(s)) );
        TSTFAILNOTE("str: [%s]\n",bufStr(s));
      }
      TSTGROUP("buf add string") {
        bufAddStr(s,"xyz");
        TSTEQINT("Len 6", 6, bufLen(s));
        TSTEQINT("Len 6", strlen(bufStr(s)), bufLen(s));
        TSTEQINT("Set properly direct access", 0, strcmp("abcxyz",bufStr(s)) );
        TSTFAILNOTE("str: [%s]\n",bufStr(s));
      }
      TSTGROUP("buf format") {
        bufFormat(s,"|%d|",123);
        TSTEQINT("Len 5", 5, bufLen(s));
        TSTEQINT("Len 5", strlen(bufStr(s)), bufLen(s));
        TSTEQINT("Set properly direct access", 0, strcmp("|123|",bufStr(s)) );
      }
    }

    TSTSECTION("buf cleanup") {
      TSTGROUP("buf clear") {
        int l = bufMax(s);
        bufClr(s);
        TSTEQINT("Len 0", 0, bufLen(s));
        TSTEQINT("Empty", 0, bufStr(s)[0]);
        TSTEQINT("Not shrunk", l, bufMax(s));
      }
      TSTGROUP("buf free") {
        TSTNEQPTR("Is Not Null", NULL, s);
        s = bufFree(s);
        TSTEQPTR("Is Null", NULL, s);
      }
    }
  }
}

