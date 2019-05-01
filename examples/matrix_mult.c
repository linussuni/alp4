#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>

int main (int argc, char *argv[])
{
  int ma[2][3];
  int mb[3][2];
  int result = 0;
  int i, j;
  int status = 0;
  pid_t pid;

  // init data
  // init random number generator
  srand ((unsigned) time (NULL));

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
      ma[i][j] = (int) rand() % 10;
	  printf ("ma : %d,%d: % d \n", i, j, ma[i][j]);
    }
  }
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 2; j++) {
      mb[i][j] = (int) rand() % 10;
	  printf ("mb : %d,%d: % d \n", i, j, mb[i][j]);
    }
  }

  pid = fork();
  if(pid == 0) {
    // child process
    for (i = 0; i < 2; i++) {
      result = 0;
      printf ("0,%d : ", i);
      for (j = 0; j < 3; j ++) {
        result += (ma[0][j] * mb[j][i]);
      }
      printf (" %d \n", result);
    }
    exit(0);
  }

  else if(pid > 0) {
    // parent process
    for (i = 0; i < 2; i++) {
      result = 0;
      printf ("1,%d : ", i);
      for (j = 0; j < 3; j ++) {
        result += (ma[1][j] * mb[j][i]);
      }
      printf (" %d \n", result);
    }
  }

  pid = wait(&status);
  printf ("\n WEXITSTATUS %d\n", WEXITSTATUS(status));
  exit(EXIT_SUCCESS);


}
