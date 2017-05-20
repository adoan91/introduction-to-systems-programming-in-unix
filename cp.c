/*************************************************************************\
*						 Copyright (C)	Michael Kerrisk, 2014.						  *
*																								  *
* This program	is	free software.	You may use, modify,	and redistribute it *
* under the	terms	of	the GNU General Public License as published by the	  *
* Free Software Foundation, either version 3	or	(at your	option) any		  *
* later version. This program	is	distributed	without any	warranty.  See	  *
* the	file COPYING.gpl-v3 for	details.												  *
\*************************************************************************/

/* 
Anthony Doan
CSC 60 Lab 4
MoWe 3:00PM - 4:15PM
Due March 17, 2015
*/

#include	<sys/stat.h>
#include	<fcntl.h>
#include	"tlpi_hdr.h"

#include <stdio.h> //
#include <errno.h> //

#ifndef BUF_SIZE			/*	Allow	"cc -D" to override definition */
#define BUF_SIZE 1024
#endif

int
main(int	argc,	char *argv[])
{
    struct stat sb; //
    int test; //

	 int inputFd, outputFd,	openFlags;
	 mode_t filePerms;
	 ssize_t	numRead;
	 char	buf[BUF_SIZE];

	 if (argc != 3	||	strcmp(argv[1], "--help") == 0)
		  usageErr("%s old-file new-file\n", argv[0]);
		  
	 if(strcmp(argv[1],argv[2])== 0){ //(2) it does not permit a file to copy to itself
	    errExit("Cannot copy a file over itself!"); //
    } //
    
    test = stat(argv[2], &sb); //
    if(S_ISDIR(sb.st_mode)){ //(1) it does not permit the second argument to be a directory
       errExit("Does not permit the second argument to be a directory!"); //
    } //

	 /* Open	input	and output files */

	 inputFd	= open(argv[1], O_RDONLY);
	 if (inputFd == -1)
		  errExit("opening file %s", argv[1]);

	 openFlags = O_CREAT	| O_WRONLY | O_TRUNC;
	 filePerms = S_IRUSR	| S_IWUSR |	S_IRGRP | S_IWGRP	|
					 S_IROTH	| S_IWOTH;		 /* rw-rw-rw- */
	 outputFd =	open(argv[2], openFlags, filePerms);
	 if (outputFd == -1)
		  errExit("opening file %s", argv[2]);

	 /* Transfer data	until	we	encounter end of input or an error */

	 while ((numRead = read(inputFd,	buf, BUF_SIZE)) >	0)
		  if (write(outputFd, buf,	numRead)	!=	numRead)
				fatal("couldn't write whole buffer");
	 if (numRead == -1)
		  errExit("read");

	 if (close(inputFd) == -1)
		  errExit("close input");
	 if (close(outputFd)	==	-1)
		  errExit("close output");

	 exit(EXIT_SUCCESS);
}
