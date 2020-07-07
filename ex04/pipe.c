#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

int main(void)
{
  pid_t pid, child1, child2;
  int fd[2], status;

  if(pipe(fd) == -1){
    perror("pipe");
    exit(EXIT_FAILURE);
  }
  child1 = fork();
  if(child1 == 0){
    dup2(fd[0], STDIN_FILENO);
    close(fd[0]);
    close(fd[1]);
    execlp("wc", "wc","-l",(char*)NULL);
  }
  child2 = fork();
  if(child2 == 0){
    dup2(fd[1], STDOUT_FILENO);
    close(fd[0]);
    close(fd[1]);
    execlp("ls", "ls",(char*)NULL);
  }
  close(fd[0]);
  close(fd[1]);
  while (wait(&status) != -1)
    ; 

  return 0;
}