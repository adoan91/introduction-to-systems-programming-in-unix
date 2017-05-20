/* 
Anthony Doan
CSC 60 Lab 6
MoWe 3:00PM - 4:15PM
Due April 17, 2015
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

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
   pid_t pid; 
   int pipepos = -1; 
   int firstpipe = -1; 
   int ripos = -1; 
   int ropos = -1; 
   FILE *rifile; 
   FILE *rofile; 
   int fd[2]; 
   int leftc = 0;
   int i;
   
   for (i = 0; i < argc; i++){
      if (strcmp(argv[i], "|") == 0){

         if (i == pipepos + 1){
            printf("ERROR - Can't start with pipe and can't be two pipes next to each other.\n");
            return;
         }
         pipepos = i;
         /* first pipe's location */
         firstpipe = (firstpipe == -1) ? i : firstpipe;
      }
      ripos = (strcmp(argv[i], "<") == 0) ? i : ripos;
      ropos = (strcmp(argv[i], ">") == 0) ? i : ropos;
   }
   
   if (pipepos != -1){
      if ((ripos != -1) && (ripos > firstpipe - 2)){
         printf("ERROR - Input redirector has to be two from the first pipe.\n");
         return;
      }
      if ((ropos != -1) && (ropos < argc - 2)){
         printf("ERROR - Output redirector has to be two from the first pipe.\n");
         return;
      }
      
      argv[pipepos] = NULL; 
      pipe(fd); /* create pipe */
      
      pid = fork();
      if (pid == -1){
         perror("Shell Program");
         return;
      }
      else if (pid == 0){ /* child */
         dup2(fd[1], fileno(stdout));
         close(fd[0]);
         close(fd[1]); 
         process_input(pipepos, argv);
      }
      else { /* parent */
         dup2(fd[0], fileno(stdin)); 
         close(fd[0]);
         close(fd[1]); 
         argv = argv + pipepos + 1; 
         argc = argc - pipepos - 1; 
      }
   }
   
   ripos = -1;
   ropos = -1;

   for (i = 0; i < argc; i++){
      ripos = (strcmp(argv[i], "<") == 0) ? i : ripos;
      ropos = (strcmp(argv[i], ">") == 0) ? i : ropos;

		if (strcmp(argv[i], "<") == 0){
			leftc++;
			if (leftc == 2){
				printf("ERROR - Can't have two input redirects on one line.\n");
				leftc = 0;
				_exit(EXIT_FAILURE);
			}
		}
   }

   if (ripos != -1){ 
      if (ripos >= argc - 1){ 
         printf("ERROR - No redirection file specified.\n");
			_exit(EXIT_FAILURE);
      } else {
         if ((rifile = fopen(argv[ripos + 1], "r")) == NULL){
            printf("Couldn't open %s for reading.\n", argv[ripos + 1]);
				_exit(EXIT_FAILURE);
         }
         else if (dup2(fileno(rifile), fileno(stdin)) == -1){
            perror("Shell Program");
				_exit(EXIT_FAILURE);
         }
         argv[ripos + 1] = NULL; 
      }
      argv[ripos] = NULL; 
   }
   if (ropos != -1){ 
      if (ropos >= argc - 1){ 
         printf("ERROR - No redirection file specified.\n");
			_exit(EXIT_FAILURE);
      } else {
         if ((rofile = fopen(argv[ropos + 1], "w")) == NULL){
            printf("Couldn't open %s for writing.\n", argv[ropos + 1]);
				_exit(EXIT_FAILURE);
         }
         else if (dup2(fileno(rofile), fileno(stdout)) == -1){
            perror("Shell Program");
				_exit(EXIT_FAILURE);
         }
         argv[ropos + 1] = NULL; 
      }
      argv[ropos] = NULL; 
   }                                             
   
   /* calling execvp */

   if(execvp(argv[0], argv) == -1){ /* execute command */
      perror("Shell Program");
		_exit(EXIT_FAILURE);
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

	struct sigaction handler;
	/* specify the action taken for the specified signal: in our case that is SIGINT */
	handler.sa_handler = SIG_IGN;
	sigemptyset(&handler.sa_mask); /* declare no signal block */
	handler.sa_flags = 0; /* no special flag */
	/* Passing: SIGINT - signal number, &handler - address of the struct sigaction handler */
	sigaction(SIGINT, &handler, NULL); 

   /* Loop forever to wait and process commands */
   while (1) {
   /* Step 1: Name your shell: csc60mshell - m for mini shell */ 
   printf("csc60mshell> "); /* print the csc60mshell */

   fgets(cmdline, MAXLINE, stdin); /* call fgets to read in command, result to store in cmdline */
  
   /* call parseline to parse the command, Result is stored in argc (argument count), and argv (argument vectors - individual parsed tokens) */

   argc = parseline(cmdline,argv);

   if(argc==0)
      continue; /* if user hits carriage return, just loop back and redisplay csc60mshell text and wait for the next command */
   if (argc >0){
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
            }else{
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
   }

   /*DONE Step 1: If user hits enter key without a command, continue to loop again at the beginning */
   /*DONE         Hint: look up for they keyword "continue" in C */
   /*DONE Step 1: Call parseline to build argc/argv: argc/argv parameters declared above */ 
   /*DONE Step 1: Handle build-in command: exit, pwd, or cd - if detect one              */
   /*DONE Step 1: Else, fork off a process */ 
   
   pid = fork();
   if (pid == -1)
      perror("Shell Program fork error");
   else if (pid == 0){

      struct sigaction handler2;
		handler2.sa_handler = SIG_DFL;
		sigemptyset(&handler2.sa_mask);
		handler2.sa_flags = 0;
		sigaction(SIGINT, &handler2, NULL);

      /* I am child process. I will execute the command, call: execvp */
      process_input(argc, argv);
   }
   else
      /* I am parent process */
      if (wait(&status) == -1)
         perror("Shell Program error");
      else
         printf("Child returned status: %d\n",status);
   }
}
