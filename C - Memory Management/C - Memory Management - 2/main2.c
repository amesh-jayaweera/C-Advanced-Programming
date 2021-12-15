#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "regions.h"

int main()
{
  fprintf(stderr, "main2.c: only regions\n");

  assert(rinit("region one", 1000));
  assert(rinit("another region", 1000));
  assert(rinit("3", 1000));
  assert(rinit("four", 1000));
  
  assert(strcmp(rchosen(), "four") == 0);
  assert(!rchoose("doesn't exist"));
  assert(strcmp(rchosen(), "four") == 0);
  assert(rchoose("region one"));
  assert(strcmp(rchosen(), "region one") == 0);
  assert(rchoose("3"));
  assert(strcmp(rchosen(), "3") == 0);
  assert(rchoose("another region"));
  assert(strcmp(rchosen(), "another region") == 0);
  
  rdump();
  
  rdestroy("another region");
  assert(rchoose("region one"));
  assert(strcmp(rchosen(), "region one") == 0);
  assert(!rchoose("another region"));

  rdestroy("3");
  assert(strcmp(rchosen(), "region one") == 0);
  assert(!rchoose("something else"));
  assert(!rchoose("3"));
  
  rdestroy("region one");
  rdestroy("four");
  assert(!rchoose("four"));

  fprintf(stderr,"\nEnd of processing.\n");

  return 0;
}