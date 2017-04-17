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
OBJ_FILES_TEST := $(filter-out obj/main.o obj/lex.yy.o obj/parser.tab.o, $(OBJ_FILES))

CFLAGS := -Wall -Wextra -I.
CFLAGS_DEBUG := $(CFLAGS) -gstabs
CFLAGS_PROD := $(CFLAGS) -O3 -Werror

$(OUT_BIN_NAME): $(OBJ_FILES) $(INC_FILES)
	$(CC) -o $@ $(OBJ_FILES) $(CFLAGS_PROD) -lfl

debug: TAGS $(OBJ_FILES) $(INC_FILES)
	$(CC) -o $(OUT_BIN_NAME) $(OBJ_FILES) $(CFLAGS_DEBUG)

## TODO: Validate that prod executable files don't end up with stabs symbols
$(OBJDIR)/%.o: $(OBJDIR) $(SRC_FILES)
	$(CC) $(CFLAGS_DEBUG) -o $@ -c $(SRCDIR)/$*.c

## We don't want warnings on when compiling generated c files.
$(OBJDIR)/lex.yy.o: $(SRCDIR)/lex.yy.c
	$(CC) -I. -O3 -lfl -o $@ -c $<

$(SRCDIR)/lex.yy.c: $(SRCDIR)/lexer.l
	flex -o $@ $<

$(SRCDIR)/parser.tab.c: $(SRCDIR)/parser.y
	bison -o $@ --defines=$(SRCDIR)/parser.tab.h $<

.PHONY: testsuite
testsuite: $(OBJ_FILES_TEST) $(INC_FILES)
	$(CC) -o $@ $(CFLAGS_DEBUG) $(TESTS_FILES) $(OBJ_FILES_TEST)
	./$@

install:
	mv -f lafscheme /usr/local/bin/
	cp -f scminit.scm /usr/local/etc/

.PHONY: TAGS
TAGS:
	find . -name "*.[chS]" -print | xargs etags --append

$(OBJDIR):
	@mkdir $(OBJDIR)

clean:
	rm -r $(OBJDIR) $(OUT_BIN_NAME)
