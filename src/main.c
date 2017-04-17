#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "inc/ast.h"
#include "inc/env.h"
#include "inc/eval.h"
#include "inc/stdmacros.h"
#include "inc/symbols.h"
#include "parser.tab.h"

#define DEFAULT_INIT_PATH "/usr/local/etc/scminit.scm"

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

static void load_init_file(struct astnode_env *env, char *path)
{
  int err;
  FILE *init_file;
  struct astnode *parsed_result;
  struct astnode *dummy;

  init_file = fopen(path, "r");
  if (init_file == NULL)
    {
      perror("Error opening init file");
      return;
    }

  yyrestart(init_file);
  if (yyparse(false, &parsed_result) != 0)
    {
      fprintf(stderr, "Error parsing init file.\n");
      return;
    }

  err = eval_many((struct astnode_pair *)parsed_result, env, &dummy);
  if (err != 0)
    {
      fprintf(stderr, "Error evaluating init file: %d\n", err);
    }
}


int main(int argc, char **argv)
{
  int err;
  struct astnode_env *env;
  struct astnode_pair *parsed_exp;
  struct astnode *evaled_exp;
  char *init_path = DEFAULT_INIT_PATH;

  if (argc >= 3 && strcmp("-i", argv[1]) == 0)
    init_path = argv[2];

  RETONERR(make_top_level_env(&env));

  load_init_file(env, init_path);

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
