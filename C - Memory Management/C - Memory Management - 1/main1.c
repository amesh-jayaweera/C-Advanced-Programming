#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "regions.h"

int main()
{
  void *blocks[125];

  fprintf(stderr, "main1.c: only blocks\n");

  rinit("region", 1000);
  
  for (int i = 0; i < 125; i++) {
    assert(blocks[i] = ralloc((i%8)+1));
  }
  assert(!ralloc(1));
  assert(rfree(blocks[0]));
  assert(!rfree(blocks[0]));
  assert(ralloc(8) == blocks[0]);
  
  for (int i = 0; i < 125; i++) {
    assert(8 == rsize(blocks[i]));
  }
  
  for (int i = 0; i < 125; i+=2) {
    assert(rfree(blocks[i]));
  }
  
  assert(!ralloc(9));
  assert(ralloc(8) == blocks[0]);
  
  for (int i = 1; i < 125; i+=4) {
    assert(rfree(blocks[i]));
  }

  assert(!ralloc(25));
  assert(ralloc(24) == blocks[4]);

  for (int i = 1; i < 125; i+=4) {
    assert(!rfree(blocks[i]));
  }

  assert(!ralloc(832));
  for (int i = 7; i < 110; i+=4) {
    assert(rfree(blocks[i]));
  }
  assert(!ralloc(833));
  assert(ralloc(832));
  
  rdump();

  fprintf(stderr,"\nEnd of processing.\n");

  return 0;
}