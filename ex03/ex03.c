#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct FileInfo {
  char* name;
  char* date;
  long size;
  long inode;
  struct FileInfo* next;
} fileInfo_t;

void addList(fileInfo_t** head, fileInfo_t* data);
void showList(fileInfo_t* head);

int main(int argc, char argv[]) {
  fileInfo_t* HEAD = NULL;
  DIR* dp;
  struct dirent* dirp;

  dp = opendir(".");
  while ((dirp = readdir(dp)) != NULL) {
    fileInfo_t* tmp = (fileInfo_t*)malloc(sizeof(fileInfo_t));
    tmp->next = NULL;
    tmp->name = (char*)malloc(sizeof(char) * (strlen(dirp->d_name) + 1));
    strcpy(tmp->name, dirp->d_name);
    addList(&HEAD, tmp);
  }
  closedir(dp);

  fileInfo_t* p = HEAD;
  while (p != NULL) {
    struct stat buf;
    if (!stat(p->name, &buf)) {
      p->inode = buf.st_ino;
      printf("%d\n", buf.st_uid);
    }
    p = p->next;
  }

  showList(HEAD);

  return 0;
}

void addList(fileInfo_t** head, fileInfo_t* data) {
  if (*head == NULL) {
    *head = data;
  } else {
    fileInfo_t* p = *head;
    while (p->next != NULL) {
      p = p->next;
    }

    p->next = data;
  }
}

void showList(fileInfo_t* head) {
  fileInfo_t* p = head;
  while (p != NULL) {
    printf("name:%*s ", 8, p->name);
    printf("inode:%d ", p->inode);
    printf("\n");
    p = p->next;
  }
}