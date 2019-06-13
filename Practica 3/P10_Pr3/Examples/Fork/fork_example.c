
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
pid_t fork(void);

int a=0;

int main(){
    int status;
    pid_t pid;
    
     pid = fork();

if (pid == 0) {
       a++;
    }
   else if (pid>0){
          a=a+2;

       wait(&status);

  }
  else{
        fprintf(stderr,"Error en fork()\n"); exit(1);
  }
 
  printf("PID=%d,a=%d\n",getpid(),a);
  exit(0);
} 
