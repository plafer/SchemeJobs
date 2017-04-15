%{
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "inc/ast.h"

int yylex(bool interactive);
void yyerror(bool interactive, struct astnode **ret, char const *);

struct astnode *new_astnode_pair(struct astnode *car, struct astnode *cdr);
struct astnode *new_astnode_emptylist(void);
%}

%param {bool interactive}
%parse-param {struct astnode **ret}

%define api.value.type {struct astnode *}

/* Includes int, symbol and boolean */
%token EXP

%%

input:		%empty
	|	list-ele { *ret = $1; }
	;

list:		'(' list-ele list-tail { $$ = new_astnode_pair($2, $3); }
	|	'(' ')'                { $$ = new_astnode_emptylist(); }
	;

list-tail:      list-ele list-tail     { $$ = new_astnode_pair($1, $2); }
	|       ')'                    { $$ = new_astnode_emptylist(); }
	;

pair:           '(' list-ele '.' list-ele ')' { $$ = new_astnode_pair($2, $4); }

list-ele:       EXP
	|	list
	|	pair
        ;

%%

void yyerror(bool interactive, struct astnode **ret, char const *arg)
{
    (void) ret;
    (void) interactive;
    printf("Parse error: %s\n", arg);
}

struct astnode *new_astnode_pair(struct astnode *car, struct astnode *cdr)
{
    struct astnode_pair *ret;

    ret = malloc(sizeof(*ret));
    // TODO: What to do on ENOMEM?

    ret->type = TYPE_PAIR;
    ret->car = car;
    ret->cdr = cdr;

    return (struct astnode *) ret;
}

struct astnode *new_astnode_emptylist(void)
{
    return new_astnode_pair(NULL, NULL);
}
