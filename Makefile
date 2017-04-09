OUT_BIN_NAME := lafscheme

SRCDIR := src
INCDIR := inc
OBJDIR := obj

CC := gcc

TESTS_FILES := $(wildcard tests/*.c)
SRC_FILES := $(wildcard $(SRCDIR)/*.c)
INC_FILES := $(wildcard $(INCDIR)/*.h)

OBJ_FILES := $(patsubst %.c,%.o,$(SRC_FILES))
OBJ_FILES := $(subst $(SRCDIR)/,$(OBJDIR)/,$(OBJ_FILES))

## Test suite provides its own main(), so we filter ours out.
OBJ_FILES_TEST := $(filter-out obj/main.o, $(OBJ_FILES))

CFLAGS := -Wall -Wextra -I.
CFLAGS_DEBUG := $(CFLAGS) -gstabs
CFLAGS_PROD := $(CFLAGS) -O3 -Werror

$(OUT_BIN_NAME): $(OBJ_FILES) $(INC_FILES)
	$(CC) -o $@ $(OBJ_FILES) $(CFLAGS_PROD)

debug: TAGS $(OBJ_FILES) $(INC_FILES)
	$(CC) -o $(OUT_BIN_NAME) $(OBJ_FILES) $(CFLAGS_DEBUG)

## TODO: Validate that prod executable files don't end up with stabs symbols
$(OBJDIR)/%.o: $(OBJDIR) $(SRC_FILES)
	$(CC) $(CFLAGS_DEBUG) -o $@ -c $(SRCDIR)/$*.c

.PHONY: testsuite
testsuite: $(OBJ_FILES_TEST) $(INC_FILES)
	$(CC) -o $@ $(CFLAGS_DEBUG) $(TESTS_FILES) $(OBJ_FILES_TEST)
	./$@

.PHONY: TAGS
TAGS:
	find . -name "*.[chS]" -print | xargs etags --append

$(OBJDIR):
	@mkdir $(OBJDIR)

clean:
	rm -r $(OBJDIR) $(OUT_BIN_NAME)
