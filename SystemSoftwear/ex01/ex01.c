#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int split(int chunkNum, int chunkLen, char** dar, char* src, int del);
extern int join(int outLen, char* dst, int joinChar, int inNum, char** inStr);
extern void chomp(char* line);

int show(int nf, char* far[]) {
  int i;
  for (i = 0; i < nf; i++) {
    printf("%3d: %3d |%s|\n", i, (int)strlen(far[i]), far[i]);
  }
}

int main() {
#define NF (10)

#if 0
 srand(time(NULL));
#endif

  char** chunk = (char**)malloc(sizeof(char*) * NF);
  if (!chunk) {
    exit(3);
  }
  for (int i = 0; i < NF; i++) {
    chunk[i] = (char*)malloc(sizeof(char) * BUFSIZ);
    if (!chunk[i]) {
      exit(5);
    }
    int m = rand() % 36 + 3;
    int j;
    for (j = 0; j < m; j++) {
      chunk[i][j] = 'A' + (rand() % 26);
    }
    chunk[i][j] = '\0';
  }

  // show(NF, chunk);

  FILE* fp;
  char str[BUFSIZ];
  /************** split ****************/
  fp = fopen("./split-test.txt", "r");
  if (fp == NULL) {
    printf("FILE OPEN ERROR\n");
    return -1;
  }

  while (fgets(str, sizeof(str), fp) != NULL) {
    chomp(str);
    printf("str = %s\n", str);
    int ik = split(NF, BUFSIZ, chunk, str, ' ');
    printf("ik %d\n", ik);
    show(ik, chunk);
    puts("");
  }

  /************** join *******************/
  char* sar[] = {"AB", "CD", "EF", "GH"};
  char* dst = (char*)malloc(sizeof(char) * NF);
  int ik;
  ik = join(8, dst, '-', 4, sar);
  printf("%s\n", dst);
  ik = join(20, dst, '-', 4, sar);
  printf("%s\n", dst);
  ik = join(3, dst, '-', 4, sar);
  printf("%s\n", dst);
  ik = join(4, dst, '-', 4, sar);
  printf("%s\n", dst);

  return 0;
}
