/* 
Anthony Doan
Partner: Navjot Singh
CSC 60 Lab 5
MoWe 3:00PM - 4:15PM
Due April 6, 2015
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define MAXLINE 80
#define MAXARGS 20

void process_input(int argc, char **argv) {
  /* Problem 1: perform system call execvp to execute command     */ 
  /*            No special operator(s) detected                   */
  /* Hint: Please be sure to review execvp.c sample program       */
  /* if (........ == -1) {                                        */  
  /*  perror("Shell Program");                                    */
  /*  _exit(-1);                                                  */
  /* }                                                            */
  /* Problem 2: Handle redirection operators: < , or  >, or both  */ 

  int i=0;
  int pos;
  int alert = 0;

  if(strcmp(argv[0], ">") == 0){
    printf("ERROR-No command. Make sure file is not overwritten\n");
    _exit(-1);
  }
  if(strcmp(argv[0], ">") == 0){
    printf("ERROR-No command. Make sure file is not overwritten\n");
    _exit(-1);
  }

  if(argc == 1){
    if(execvp(argv[0], argv) == -1){ /* execute command */
      perror("Shell Program");
      _exit(-1);
    }
  }

  if(argc == 2){
  
    for(i=0;i<argc; i++){
       if(strcmp(argv[i], ">") == 0){
           /* if you found it, set found pos to the i location */
           pos = i;
   
         if(argv[i+1]==NULL){
           printf("ERROR-No redirection file specified\n");
           _exit(-1);
         }
       }
       
       if(strcmp(argv[i], "<") == 0){
           /* if you found it, set found pos to the i location */
           pos = i;
   
         if(argv[i+1]==NULL){
           printf("ERROR-No redirection file specified\n");
           _exit(-1);
         }
       }
    }

    if(execvp(*argv, argv)<0){
      perror("Shell Program");
      _exit(-1);
    }
  }

  if(argc > 2){
    
    for(i=0; i<argc; i++){
      /* compare argv[i] with ">" or "<" */
      if(strcmp(argv[i], ">") == 0){
        /* if you found it, set found pos to the i location */
        pos = i;

      if(argv[i+1]==NULL){
          printf("ERROR-No redirection file specified\n");
          _exit(-1);
        }
      }
      if(strcmp(argv[i], "<") == 0){
        /* if you found it, set found pos to the i location */
        pos = i;
        alert++;

        if(alert==2){
          printf("ERROR-Can't have two input redirects on one line\n");
          _exit(-1);
        }

        if(argv[i+1]==NULL){
          printf("ERROR-No redirection file specified\n");
          _exit(-1);
        }
      }
    }

    if(alert==1){
      /* open a filename argv[pos+1] to get to filename */

      int fileId = open(argv[pos+1],O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | O_RDWR); 
    

      /* run dup2 */

      dup2(1, fileId); /* copy fileID to stdout */

      /* close the unneeded files */

      close( fileId ); /* close fileId - no longer need */ 

      /* Assign NULL value to argv[pos] by this code argv[pos]= NULL; */

      argv[pos]= NULL;
    }

    if(alert==0){
      /* open a filename argv[pos+1] to get to filename that is "a.txt" */

      int fileId = open(argv[pos+1],O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | O_RDWR); 
    

      /* run dup2 */

      dup2(fileId, 1); /* copy fileID to stdout */

      /* close the unneeded files */

      close( fileId ); /* close fileId - no longer need */ 

      /* Assign NULL value to argv[pos] by this code argv[pos]= NULL; */

      argv[pos]= NULL;
    }


    /* calling execvp */

    if(execvp(argv[0], argv) == -1){ /* execute command */
      perror("Shell Program");
    }

  }

}

/* ----------------------------------------------------------------- */
/*                  parse input line into argc/argv format           */
/* ----------------------------------------------------------------- */
int parseline(char *cmdline, char **argv)
{
  int count = 0;
  char *separator = " \n\t";
  argv[count] = strtok(cmdline, separator);
  while ((argv[count] != NULL) && (count+1 < MAXARGS)) {
   argv[++count] = strtok((char *) 0, separator);
  }
  return count;
}
/* ----------------------------------------------------------------- */
/*                  The main program starts here                     */
/* ----------------------------------------------------------------- */
int main(void)
{
 char cmdline[MAXLINE];
 char *argv[MAXARGS];
 int argc;
 int status;
 pid_t pid;
 
 char *temp;
 char tempbuf[256];

 /* Loop forever to wait and process commands */
 while (1) {
  /* Step 1: Name your shell: csc60mshell - m for mini shell */ 
  printf("csc60mshell> "); /* print the csc60mshell */

  fgets(cmdline, MAXLINE, stdin); /* call fgets to read in command, result to store in cmdline */
  
  //

  /* call parseline to parse the command, Result is stored in argc (argument count), and argv (argument vectors - individual parsed tokens) */

  argc = parseline(cmdline,argv);

  if(argc==0)
    continue; /* if user hits carriage return, just loop back and redisplay csc60mshell text and wait for the next command */

  if(argc == 1 && strcmp(argv[0], "exit") == 0){
    exit(-1);
  }
  else if(argc == 1 && strcmp(argv[0], "pwd") == 0){
    getcwd(tempbuf,256);
  }
  else if(strcmp(argv[0], "cd") == 0){

    // parameter is given -- change to that directory
    // i.e cd /home/ftp/nguyendh/hw5. User provides directory name.

    if(argc == 2){
      if(chdir(argv[1]) == 0){
        printf("Directory changed to %s\n", argv[1]);
      } else{
        perror(argv[1]);
      }
    }else{
      temp = getenv("HOME");
      chdir(temp);
      printf("Changed to user's home directory.\n");
    } //update PWD environment variable with the new directory
      // This is so that if you type in pwd later, you have the correct data
      // to display by using getenv("PWD") function call.

    getcwd(tempbuf,256);

    setenv("PWD", tempbuf, 1);

  }

  //

  /*DONE Step 1: If user hits enter key without a command, continue to loop again at the beginning */
  /*DONE         Hint: look up for they keyword "continue" in C */
  /*DONE Step 1: Call parseline to build argc/argv: argc/argv parameters declared above */ 
  /*DONE Step 1: Handle build-in command: exit, pwd, or cd - if detect one              */
  /*DONE Step 1: Else, fork off a process */ 
  pid = fork();
  if (pid == -1) 
    perror("Shell Program fork error");
  else if (pid == 0) 
    /* I am child process. I will execute the command, call: execvp */
    process_input(argc, argv);
  else 
    /* I am parent process */
    if (wait(&status) == -1)
      perror("Shell Program error");
    else
      printf("Child returned status: %d\n",status);
 }
}
