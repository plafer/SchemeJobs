#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "inc/symbols.h"

struct sym {
  char *buffer;
};

#define NTABLES_MAX 10
#define NENTRIES ((uint32_t) (4096 / sizeof(struct sym)))
#define TABLESZ ((uint32_t) (NENTRIES * sizeof(struct sym)))

static struct sym *tables[NTABLES_MAX];
static uint32_t ntables;
static uint32_t nentries_lasttable;

static bool is_valid_index(void *index)
{
  uint32_t i;

  for (i = 0; i < ntables; i++)
    {
      if (index >= (void *)tables[i] && index < (void *)(tables[i] + NENTRIES))
	{
	  // Make sure it's aligned
	  if (((uintptr_t)index - (uintptr_t) tables[i]) % sizeof(struct sym) == 0)
	    return true;

	  return false;
	}
    }

  return false;
}

// Searches through all the symbol tables to find symval.
// Returns NULL if none were found.
static struct sym *find_sym(char *symval, size_t len)
{
  uint32_t i;

  for (i = 0; i < ntables; i++)
    {
      uint32_t j;
      uint32_t nentries;

      nentries = (i == ntables - 1) ? nentries_lasttable : NENTRIES;
      for (j = 0; j < nentries; j++)
	{
	  // With induction variable elimination optimization, accessing
	  // `tables` like this is just as efficient as with a pointer.
	  if (strncmp(tables[i][j].buffer, symval, len) == 0 &&
	      strlen(tables[i][j].buffer) == len)
	    return &tables[i][j];
	}
    }

  return NULL;
}

static struct sym *next_avail_index(void)
{
  struct sym *curtable;

  if (ntables == 0 || nentries_lasttable == NENTRIES)
    {
      ntables += 1;
      if (ntables > NTABLES_MAX)
	return NULL;

      tables[ntables - 1] = malloc(TABLESZ);
      if (tables[ntables - 1] == NULL)
	return NULL;

      nentries_lasttable = 0;
    }

  curtable = tables[ntables - 1];

  return curtable + nentries_lasttable++;
}

int putsym(char *symval_start, char *symval_end, void **index)
{
  struct sym *symindex;
  char *symbuffer;
  uint32_t bufsz;

  if (symval_start == NULL || symval_end == NULL || symval_start > symval_end)
    return EINVAL;

  symindex = find_sym(symval_start, symval_end - symval_start + 1);
  if (symindex != NULL)
    {
      if (index != NULL)
	*index = symindex;
      return 0;
    }

  bufsz = symval_end - symval_start + 2;
  symbuffer = malloc(bufsz);
  if (symbuffer == NULL)
    return ENOMEM;
  memcpy(symbuffer, symval_start, bufsz - 1);
  symbuffer[bufsz - 1] = '\0';

  symindex = next_avail_index();
  if (symindex == NULL)
    {
      free(symbuffer);
      return ENOMEM;
    }

  symindex->buffer = symbuffer;
  if (index != NULL)
    *index = symindex;

  return 0;
}

int getsym(void *index, const char **symval)
{
  if (symval == NULL || !is_valid_index(index))
    return EINVAL;

  *symval = ((struct sym *) index)->buffer;

  return 0;
}
