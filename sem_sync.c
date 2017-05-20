/* 
Anthony Doan
CSC 60 Take Home Final (part one)
MoWe 3:00PM - 4:15PM
Due May 11, 2015
*/

/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2015.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

#include "curr_time.h"                      /* Declaration of currTime() */
#include "tlpi_hdr.h"
#include "semun.h"		//
#include "svshm_xfr.h"		//
#include <sys/types.h>		//
#include <sys/sem.h>			//
#include <sys/stat.h>		//

int
main(int argc, char *argv[])
{
    int semid;					//
    union semun arg;			//

    int pfd[2];                             /* Process synchronization pipe */
    int j, dummy;

    semid = semget(SEM_KEY, 1, IPC_CREAT | OBJ_PERMS); //CREATE ONE SEMAPHORE
    if (semid == -1)												 //
        errExit("semid");										//
    arg.val = 0;										//
    semctl(semid, 0, SETVAL, arg); 				//INITIAL VALUE 0
    printf("Semaphore ID = %d\n", semid);		//


    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s sleep-time...\n", argv[0]);

    setbuf(stdout, NULL);                   /* Make stdout unbuffered, since we
                                               terminate child with _exit() */
    //printf("%s  Parent started\n", currTime("%T"));

    struct sembuf sop;				//
    sop.sem_num = 0;					//
    sop.sem_op = ((argc-1)*-1); // PARENT PUTTING NEGATIVE INTO
    sop.sem_flg = 0;					// SEMAPHORE TO MAKE A INITIAL BLOCK

    if (pipe(pfd) == -1)
        errExit("pipe");

    for (j = 1; j < argc; j++) {
        switch (fork()) {
        case -1:
            errExit("fork %d", j);

        case 0: /* Child */
            if (close(pfd[0]) == -1)        /* Read end is unused */
                errExit("close");

            /* Child does some work, and lets parent know it's done */

            struct sembuf sops;		//
            sops.sem_flg = 0;			//
            sops.sem_num = 1;			//
            sops.sem_op = 1;			// ADDING 1 TO SEMAPHORE

            sleep(getInt(argv[j], GN_NONNEG, "sleep-time"));
                                            /* Simulate processing */
            printf("%s  Child %d unlocking (PID=%ld)\n",
                    currTime("%T"), j, (long) getpid());
            if (close(pfd[1]) == -1)
                errExit("close");

            /* Child now carries on to do other things... */

            _exit(EXIT_SUCCESS);

        default: /* Parent loops to create next child */
            break;
        }
    }

    /* Parent comes here; close write end of pipe so we can see EOF */

    if (close(pfd[1]) == -1)                /* Write end is unused */
        errExit("close");

    /* Parent may do other work, then synchronizes with children */

    if (read(pfd[0], &dummy, 1) != 0)
        fatal("parent didn't get EOF");
    printf("%s  all obstacles removed, parent proceeds\n", currTime("%T"));

    /* Parent can now carry on to do other things... */
    /*
    if(semctl(semid, 0, IPC_RMID)== -1)
        errExit("semctl");
    */
    
    semctl(semid, 0, IPC_RMID);                             // REMOVE
    printf("sem id (%d) successfully removed\n", semid);		//

    exit(EXIT_SUCCESS);
}
