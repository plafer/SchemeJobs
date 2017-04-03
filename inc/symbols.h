#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdint.h>

// Insert a symbol [symval_start, symval_end] into the symbol table. `putsym`
// will allocate its own buffer for `symval` and copy its contents into it; it
// will not touch the memory between [symval_start, symval_end]. If `index` is
// not NULL, the index of `symval` in the table is written in `index`. If the
// symbol is already in the table, its index is simply written in `index`.
// Possible errors:
// + EINVAL: symval_start > symval_end, or either are NULL
// + ENOMEM: Failed to allocate memory for the table or internal buffer.
int putsym(char *symval_start, char *symval_end, void **index);

// Retrieve a symbol from the symbol table at index `index`. `symval` is
// modified to point to the internal buffer that contains the symbol value.
// Possible errors:
// + EINVAL: There is no symbol at index `index`, or `symval` is NULL
int getsym(void *index, const char **symval);

#endif
