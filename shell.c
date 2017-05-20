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
   int countLess = 0;
   int i;
/*   for (i=0;i<argc;i++)
   {
	if(strcmp(argv[0],"<")==0)
	   countLess +=1;
   }
   if (countLess >2)
   {
	printf("Error");
	_exit(EXIT_FAILURE);
   }
   else if((strcmp(argv[0],">")==0) || (strcmp(argv[0],"<")==0))
   {
	printf("Error");
	_exit(EXIT_FAILURE);
   }   
   else if((strcmp(argv[argc],">")==0) || (strcmp(argv[argc],"<")==0))
   {
	printf("Error");
	_exit(EXIT_FAILURE);
   }
 
   int i;
*/   for (i = 0; i < argc ; i++)
   {
        if (strcmp(argv[i], ">") == 0)
	{
	    int pos = i;
	    int fd = creat(argv[pos+1], 0644 );
            dup2(fd,1);
	    close(fd);
	    argv[pos] = NULL;
        }
        else if (strcmp(argv[i], "<") == 0)
	{
	    int pos = i;
	    int fd = open(argv[pos+1], O_RDWR );
            dup2(fd,0);
	    close(fd);
	    argv[pos] = NULL;
        }
   } 
  /* Step 1: Handle build-in command: exit, pwd, or cd - if detect one              */
   if(execvp(argv[0],argv) == -1) {   
     perror("Shell Program");       
     _exit(0); 
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
 char tempbuf[256];
 char *temp;
 int argc;
 int status;
 pid_t pid;

 /* Loop forever to wait and process commands */
 while (1) {
  /* Step 1: Name your shell: csc60mshell - m for mini shell */ 
  printf("Lab5shell> ");
  fgets(cmdline, MAXLINE, stdin);
  argc = parseline(cmdline,argv);
  /* Step 1: If user hits enter key without a command, continue to loop again at the beginning */
	if(argc == 0)
		continue;

  /*         Hint: look up for they keyword "continue" in C */
  /* Step 1: Call parseline to build argc/argv: argc/argv parameters declared above */
  /* Step 1: Handle build-in command: exit, pwd, or cd - if detect one              */
	if (strcmp (argv[0],"exit")==0)
		exit(0);
	else if (strcmp (argv[0], "pwd")==0){	
		getcwd(tempbuf, 256);
		printf("%s\n",tempbuf);
	}
	else if (strcmp (argv[0], "cd") ==0){
		if (argc == 2){
			if (chdir(argv[1])==0){
				chdir(argv[1]);
				printf ("Changed to: %s\n",argv[1]);
				
			}
			else 
			perror(argv[1]);
		}
		else{
			temp = getenv("HOME");
			chdir(temp);
		}
		getcwd(tempbuf,256);
		setenv("pwd",tempbuf,1);
	}
        else {
	
  /* Step 1: Else, fork off a process */ 
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
}
