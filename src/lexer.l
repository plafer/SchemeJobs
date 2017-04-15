%{
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "inc/ast.h"
#include "inc/gc.h"
#include "inc/symbols.h"

#include "parser.tab.h"

#define YY_DECL int yylex(bool interactive)

int got_int(void);
int got_boolean(void);
int got_sym(void);
%}

INT        [0-9]+
BOOLEAN    #[tf]
SYM        [a-zA-Z_\-]+

%%

\n          { if (interactive) return 0;}
[\t\x20]    { /* Skip whitespace */}
[()]           { return yytext[0]; }

{INT}            { return got_int(); }
{BOOLEAN}        { return got_boolean(); }
{SYM}            { return got_sym(); }

%%

int got_int(void)
{
    int err;
    struct astnode_int *num;

    err = alloc_astnode(TYPE_INT, (struct astnode **) &num);
    if (err != 0)
	perror("alloc_astnode - got_boolean:");

    num->intval = atoi(yytext);

    yylval = (struct astnode *) num;

    return EXP;
}

int got_boolean(void)
{
    int err;
    struct astnode_boolean *exp;

    err = alloc_astnode(TYPE_BOOLEAN, (struct astnode **) &exp);
    if (err != 0)
	perror("alloc_astnode - got_boolean:");

    if (strcmp("#t", yytext) == 0)
	exp->boolval = true;
    else
	exp->boolval = false;

    yylval = (struct astnode *) exp;

    return EXP;
}

int got_sym(void)
{
    int err;
    struct astnode_sym *sym;

    err = alloc_astnode(TYPE_SYM, (struct astnode **) &sym);
    if (err != 0)
	perror("alloc_astnode - got_sym:");

    err = putsym(yytext, yytext + strlen(yytext) - 1, &sym->symi);
    if (err != 0)
	{
	    fprintf(stderr, "Failed to add '%s' to symbol table.\n", yytext);
	}

    yylval = (struct astnode *) sym;

    return EXP;
}
