#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "regions.h"

int main()
{
  char name[6];
  char *ptr1, *ptr2, *ptr3, *ptr4, *ptr5;
  
  fprintf(stderr, "main3.c: ICYMI\n");

  strcpy(name, "one");
  assert(rinit(name, 16));
  strcpy(name, "two");
  assert(rinit(name, 16));
  strcpy(name, "three");
  assert(rinit(name, 16));
  
  assert(strcmp(rchosen(), "three") == 0);
  assert(rchoose("one"));
  assert(strcmp(rchosen(), "one") == 0);
  
  assert(NULL != (ptr1 = ralloc(8)));
  assert(NULL != (ptr2 = ralloc(8)));
  for (int i = 0; i < 16; i++) {
    assert(0 == ptr1[i]);
  }
  strcpy(ptr1, "abcdefghijklmno");
  assert(0 == strcmp(ptr2, "ijklmno"));
  ptr1[7] = '\0';
  assert(0 == strcmp(ptr1, "abcdefg"));

  assert(rchoose("two"));
  assert(NULL != (ptr3 = ralloc(16)));
  for (int i = 0; i < 16; i++) {
    assert(0 == ptr3[i]);
  }
  strcpy(ptr3, "123456789012345");
  assert(0 == strcmp(ptr1, "abcdefg"));
  assert(0 == strcmp(ptr2, "ijklmno"));
  assert(0 == strcmp(ptr3, "123456789012345"));

  assert(rchoose("three"));
  assert(NULL != (ptr4 = ralloc(8)));
  strcpy(ptr4, "!!!!!!!");
  assert(NULL != (ptr5 = ralloc(8)));
  strcpy(ptr5, "???????");
  assert(rfree(ptr4));
  assert(ptr4 == ralloc(8));
  for (int i = 0; i < 8; i++) {
    assert(0 == ptr4[i]);
  }
  assert(0 == strcmp(ptr5, "???????"));

  rdump();

  assert(0 == strcmp(ptr3, "123456789012345"));
  rdestroy("two");

  assert(0 == strcmp(ptr5, "???????"));
  rdestroy("three");
  
  assert(0 == strcmp(ptr2, "ijklmno"));
  assert(0 == strcmp(ptr1, "abcdefg"));
  assert(0 == strcmp(ptr1, "abcdefg"));
  assert(0 == strcmp(ptr2, "ijklmno"));
  
  
  fprintf(stderr,"\nEnd of processing.\n");

  return 0;
}