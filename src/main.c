#include <errno.h>
#include <stdio.h>
#include "inc/ast.h"
#include "inc/env.h"
#include "inc/eval.h"
#include "inc/stdmacros.h"
#include "inc/symbols.h"
#include "parser.tab.h"

void yyrestart(FILE *);

static void print_exp(struct astnode *root);

static void print_sym(struct astnode_sym *sym)
{
  const char *symval;

  if (getsym(sym->symi, &symval) != 0)
    {
      fprintf(stderr, "getsym returned non-zero.\n");
      return;
    }

  printf("%s", symval);
}

static void print_boolean(struct astnode_boolean *boolean)
{
  if (boolean->boolval)
    printf("#t");
  else
    printf("#f");
}

static void print_pair_elements(struct astnode_pair *pair)
{
  if (pair->car != NULL)
    {
      print_exp(pair->car);
    }

  if (pair->cdr != NULL)
    {
      if (is_empty_list((struct astnode *)pair->cdr))
	return;

      printf(" ");

      if (pair->cdr->type == TYPE_PAIR)
	{
	  print_pair_elements((struct astnode_pair *) pair->cdr);
	}
      else
	{
	  printf(". ");
	  print_exp(pair->cdr);
	}
    }
}


static void print_exp(struct astnode *root)
{
  switch(root->type)
    {
    case TYPE_SYM:
      print_sym((struct astnode_sym *) root);
      break;
    case TYPE_INT:
      printf("%d", ((struct astnode_int *)root)->intval);
      break;
    case TYPE_BOOLEAN:
      print_boolean((struct astnode_boolean *) root);
      break;
    case TYPE_PAIR:
      printf("(");
      print_pair_elements((struct astnode_pair *)root);
      printf(")");
      break;
    case TYPE_ENV:
      printf("<env>");
      break;
    case TYPE_KEYWORD:
      printf("<syntax keyword>");
      break;
    case TYPE_PRMTPROC:
      printf("<primitive proc>");
      break;
    case TYPE_COMPPROC:
      printf("<compound proc>");
      break;
    default:
      printf("<Unknown type %d>", root->type);
    }
}


int main()
{
  int err;
  struct astnode_env *env;
  struct astnode_pair *parsed_exp;
  struct astnode *evaled_exp;

  RETONERR(make_top_level_env(&env));

  printf("Welcome back!\n");
  printf("Keep hacking, keep rocking \\m/\n\n");

  while(1)
    {
      printf(">> ");
      yyrestart(stdin);
      err = yyparse(true, (struct astnode **)&parsed_exp);
      if (err != 0)
	{
	  fprintf(stderr, "There was an error when parsing.\n");
	  continue;
	}
      if ((err = eval_many(parsed_exp, env, &evaled_exp)) != 0)
	{
	  if (err == EBADMSG)
	    printf("Invalid input.");
	  else
	    printf("Error in evaluating expression: %d", err);
	}
      else
	print_exp(evaled_exp);
      printf("\n");
    }

  return 0;
}
