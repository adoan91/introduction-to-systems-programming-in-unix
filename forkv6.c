/* 
Anthony Doan
CSC 60 Lab 4
MoWe 3:00PM - 4:15PM
Due May 1, 2015
*/


/*************************************************************************\
 This program is to demostrate a usage of binary semaphore.
 In this context, we show that parent and child to alternate printing
 a text message to a screen.  
 Doan Nguyen: 4/19/15 
 \*************************************************************************/
#include "semun.h"              /* Definition of semun union */
#include "svshm_xfr.h"
#define PARENT 0
#define CHILD 1
int
main(int argc, char *argv[])
{
    int semid;
    union semun dummy,arg;
    int i;
    /* Create 2 semaphores */
    semid = semget(SEM_KEY, 2, IPC_CREAT | OBJ_PERMS);
    printf("semid is: %d\n", semid);
    /* Initialize parent semaphore to available */
    arg.val = 0;
    semctl(semid,PARENT,SETVAL,arg);
    /* Initialize child semaphore to in use */
    arg.val = 1;
    semctl(semid,CHILD,SETVAL,arg);

    int pid = fork();
    if (pid > 0) { /* parent code starts here  */
       struct sembuf sops;
       sops.sem_flg = 0;
       for (i=0;i<10;i++) {
          // ***********
          // reserve parent sem
          // ***********
          sops.sem_num = PARENT; /* Wait for our turn */
          sops.sem_op = -1;
          while (semop(semid, &sops, 1) == -1)
            if (errno != EINTR || !bsRetryOnEintr)
               errExit("releaseSem");

          printf("\t\t\tPARENT %d\n", i);
          // ***********
          // release child sem
          // ***********
          sops.sem_num = CHILD; /* Give child a turn */
          sops.sem_op = 1;
          semop(semid,&sops,1);
       }
       sleep(1);
       sops.sem_num = PARENT;
       sops.sem_op = -1;
       semop(semid,&sops,1);
       printf("Parent cleaning up semamphores & done \n");
       if (semctl(semid, 0, IPC_RMID, dummy) == -1) errExit("semctl");
       exit(EXIT_SUCCESS);
    }
    else { /* child code starts here */
       struct sembuf sops;
       sops.sem_flg = 0;
       for (i=0;i<10;i++) {
          // ***********
          // reserve child sem
          // ***********
          sops.sem_num = CHILD;  /* Wait for our turn */
          sops.sem_op = -1;
          while (semop(semid, &sops, 1) == -1)
            if (errno != EINTR || !bsRetryOnEintr)
               errExit("releaseSem");
          printf("CHILD %d\n", i );
#ifdef __DEBUG__
          sleep(40);
#endif
          // ***********
          // release parent sem
          // ***********
          sops.sem_num = PARENT; /* Give parent a turn */
          sops.sem_op = 1;
          semop(semid,&sops,1);
       }
       sops.sem_num = PARENT;
       sops.sem_op = 1;
       semop(semid,&sops,1);
       _exit(EXIT_SUCCESS);
    }
}


