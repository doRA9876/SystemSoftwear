#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include "find-argv.h"

int parseCmdArgs(int argc, char* argv[], findArgs_t* fa);
int findByName(char* name, char* base_path, int opt_print);

int main(int argc, char* argv[]) {
  findArgs_t fa;
  if (1 < argc) {
    if (parseCmdArgs(argc, argv, &fa)) {
      printf("error parsed args.\n");
      exit(EXIT_FAILURE);
    }
  } else {
    printf("Usage: $find [option] [starting path..] [expression]\n");
    exit(EXIT_FAILURE);
  }

  printf("name = %d, %s\n", fa.option_bit & 0b001, fa.file_name);
  printf("type = %d, %d\n", fa.option_bit & 0b010, fa.type_opt);
  printf("print = %d\n", fa.option_bit & 0b100);

  char** p = fa.path;
  for (int i = 0; i < fa.path_num; i++)
  {
    printf("arg: %s\n", fa.path[i]);
  }

  // findByName()  

  return 0;
}

int findByName(char* name, char* base_path, int opt_print)
{
  DIR* dp;

  dp = opendir(base_path);
  if(dp == NULL)
  {
    perror(base_path);
    return -1;
  }
}