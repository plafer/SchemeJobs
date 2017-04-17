%{
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "inc/ast.h"
#include "inc/gc.h"

int yylex(bool interactive);
void yyerror(bool interactive, struct astnode **ret, char const *);

static struct astnode *
new_astnode_pair(struct astnode *car, struct astnode *cdr);

static struct astnode *
new_astnode_emptylist(void);

static struct astnode_pair *
add_to_list(struct astnode *ele, struct astnode_pair *tail);

static struct astnode_pair *list_tail;
%}

%param {bool interactive}
%parse-param {struct astnode **ret}

%initial-action
{
    // This node will be converted into an actual node in add_to_list
    *ret = new_astnode_emptylist();
    list_tail = (struct astnode_pair *) *ret;
}

%define api.value.type {struct astnode *}

/* Includes int, symbol and boolean */
%token EXP

%%
input:		%empty
	|	list-ele               { if ((list_tail = add_to_list($1, list_tail)) == NULL)
			                   return 2; }
	|	input list-ele        { if ((list_tail = add_to_list($2, list_tail)) == NULL)
			                   return 2; }
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
static struct astnode *
new_astnode_pair(struct astnode *car, struct astnode *cdr)
{
    struct astnode_pair *ret;

    ret = malloc(sizeof(*ret));
    // TODO: What to do on ENOMEM?

    ret->type = TYPE_PAIR;
    ret->car = car;
    ret->cdr = cdr;

    return (struct astnode *) ret;
}

static struct astnode *
new_astnode_emptylist(void)
{
    return new_astnode_pair(NULL, NULL);
}

static struct astnode_pair *
add_to_list(struct astnode *ele, struct astnode_pair *tail)
{
    tail->cdr = new_astnode_emptylist();
    if (tail->cdr == NULL)
	return NULL;

    tail->car = ele;

    return (struct astnode_pair *) tail->cdr;
}
