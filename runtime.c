/***************************************************************************
 *  Title: Runtime environment 
 * -------------------------------------------------------------------------
 *    Purpose: Runs commands
 *    Author: Stefan Birrer
 *    Version: $Revision: 1.3 $
 *    Last Modification: $Date: 2009/10/12 20:50:12 $
 *    File: $RCSfile: runtime.c,v $
 *    Copyright: (C) 2002 by Stefan Birrer
 ***************************************************************************/
/***************************************************************************
 *  ChangeLog:
 * -------------------------------------------------------------------------
 *    $Log: runtime.c,v $
 *    Revision 1.3  2009/10/12 20:50:12  jot836
 *    Commented tsh C files
 *
 *    Revision 1.2  2009/10/11 04:45:50  npb853
 *    Changing the identation of the project to be GNU.
 *
 *    Revision 1.1  2005/10/13 05:24:59  sbirrer
 *    - added the skeleton files
 *
 *    Revision 1.6  2002/10/24 21:32:47  sempi
 *    final release
 *
 *    Revision 1.5  2002/10/23 21:54:27  sempi
 *    beta release
 *
 *    Revision 1.4  2002/10/21 04:49:35  sempi
 *    minor correction
 *
 *    Revision 1.3  2002/10/21 04:47:05  sempi
 *    Milestone 2 beta
 *
 *    Revision 1.2  2002/10/15 20:37:26  sempi
 *    Comments updated
 *
 *    Revision 1.1  2002/10/15 20:20:56  sempi
 *    Milestone 1
 *
 ***************************************************************************/
#define __RUNTIME_IMPL__

/************System include***********************************************/
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

/************Private include**********************************************/
#include "runtime.h"
#include "io.h"

/************Defines and Typedefs*****************************************/
/*  #defines and typedefs should have their names in all caps.
 *  Global variables begin with g. Global constants with k. Local
 *  variables should be in all lower case. When initializing
 *  structures and arrays, line everything up in neat columns.
 */

/************Global Variables*********************************************/

#define NBUILTINCOMMANDS (sizeof BuiltInCommands / sizeof(char*))

bgjobL* bgjobs = NULL;
int fgrun = 0;

/************Function Prototypes******************************************/
/* run command */
static void
RunCmdFork(commandT*, bool);
/* runs an external program command after some checks */
static void
RunExternalCmd(commandT*, bool);
/* resolves the path and checks for exutable flag */
static bool
ResolveExternalCmd(commandT*);
/* forks and runs a external program */
static void
Exec(commandT*, bool);
/* runs a builtin command */
/* checks whether a command is a builtin command */
/************External Declaration*****************************************/

/**************Implementation***********************************************/


/*
 * RunCmd
 *
 * arguments:
 *   commandT *cmd: the command to be run
 *
 * returns: none
 *
 * Runs the given command.
 */
void
RunCmd(commandT* cmd)
{
  RunCmdFork(cmd, TRUE);
} /* RunCmd */


/*
 * RunCmdFork
 *
 * arguments:
 *   commandT *cmd: the command to be run
 *   bool fork: whether to fork
 *
 * returns: none
 *
 * Runs a command, switching between built-in and external mode
 * depending on cmd->argv[0].
 */
void
RunCmdFork(commandT* cmd, bool to_fork)
{
    int i,pid=0,readin = 0, readout = 0;
    sigset_t sigset;
    for (i = 0; cmd->argv[i] != 0; i++)
    {
		if( *(cmd->argv[i]) == '>'){
				readin = i;
		}else if(*(cmd->argv[i]) == '<'){
				readout = i;
		}			
    }
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGCHLD);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
	if( (pid=fork()) ){
        fgjob.pid = pid;
        fgrun = pid;
				int name_offset = 0;
				int k = 0;
				for(k=0;cmd->argv[k] !=0;k++){
					sprintf(fgjob.fg_js+name_offset,"%s ",cmd->argv[k]);
					name_offset+=strlen(cmd->argv[k]);
				}
				name_offset++;
				fgjob.fg_js[name_offset] = '\0';
				printf("%s \n",fgjob.fg_js);
        printf("fgpid = %d\n", fgjob.pid);
        sigprocmask(SIG_UNBLOCK, &sigset, NULL);
		while (fgrun == fgjob.pid) {
       		sleep(1);
     	}
    printf("fgpid = %d\n", fgjob.pid);
	}else{
		if(readin){
			int f = open(cmd->argv[readin+1],O_CREAT|O_RDWR,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
			close(1);
			dup(f);
			cmd->argv[readin] = NULL;
		}else if(readout){
			int f = open(cmd->argv[readout+1],O_RDWR);
			close(0);
			dup(f);
			cmd->argv[readout] = NULL;
		}						
        setpgid(0,0);
        sigprocmask(SIG_UNBLOCK, &sigset, NULL);
		if(execv(cmd->name,cmd->argv)<0){
			printf("this failed y'all\n");
			perror("zis is ze problem:");
		}
	}
} /* RunCmdFork */


/*
 * RunCmdBg
 *
 * arguments:
 *   commandT *cmd: the command to be run
 *
 * returns: none
 *
 * Runs a command in the background.
 */
void
RunCmdBg(commandT* cmd)
{
	pid_t pid;
	if( (pid=fork())){
		char* bg_js = malloc(512);
		int name_offset = 0;
		int k = 0;
		for(k=0;cmd->argv[k] !=0;k++){
			sprintf(bg_js+name_offset,"%s ",cmd->argv[k]);
			name_offset+=strlen(cmd->argv[k]);
		}
		name_offset++;
		bg_js[name_offset] = '\0';
		printf("%s \n",bg_js);
		Push(pid);

	}else{
		if(execv(cmd->name,cmd->argv)<0){
			printf("this failed y'all\n");
			perror("zis is ze problem:");
		}
	}

  // TODO
} /* RunCmdBg */


/*
 * RunCmdPipe
 *
 * arguments:
 *   commandT *cmd1: the commandT struct for the left hand side of the pipe
 *   commandT *cmd2: the commandT struct for the right hand side of the pipe
 *
 * returns: none
 *
 * Runs two commands, redirecting standard output from the first to
 * standard input on the second.
 */
void
RunCmdPipe(commandT* cmd1, commandT* cmd2)
{	
	int pid1,pid=18,cmd1_to_cmd2[2];	
	pipe(cmd1_to_cmd2);
	if((pid1=fork()) == 0){
		close(1);				
		dup(cmd1_to_cmd2[1]);
		close(cmd1_to_cmd2[0]);
		if(execv(cmd1->name,cmd1->argv)<0){
			printf("this failed y'all\n");
			perror("zis is ze problem:");
		}
	}else if( (pid=fork()) == 0){
			close(0);
			dup(cmd1_to_cmd2[0]);
			close(cmd1_to_cmd2[1]);
			if(execv(cmd2->name,cmd2->argv)<0){
				printf("this failed y'all\n");
				perror("zis is ze problem:");
			}
	}else{
			close(cmd1_to_cmd2[0]);
			close(cmd1_to_cmd2[1]);
      fgjob.pid = pid;
			fgjob.pipes = 1;
      fgrun = pid;
      while (fgrun == fgjob.pid) {
          sleep(1);
      }
	}
} /* RunCmdPipe */


/*
 * RunCmdRedirOut
 *
 * arguments:
 *   commandT *cmd: the command to be run
 *   char *file: the file to be used for standard output
 *
 * returns: none
 *
 * Runs a command, redirecting standard output to a file.
 */
void
RunCmdRedirOut(commandT* cmd, char* file)
{
} /* RunCmdRedirOut */


/*
 * RunCmdRedirIn
 *
 * arguments:
 *   commandT *cmd: the command to be run
 *   char *file: the file to be used for standard input
 *
 * returns: none
 *
 * Runs a command, redirecting a file to standard input.
 */
void
RunCmdRedirIn(commandT* cmd, char* file)
{
}  /* RunCmdRedirIn */


/*
 * RunExternalCmd
 *
 * arguments:
 *   commandT *cmd: the command to be run
 *   bool fork: whether to fork
 *
 * returns: none
 *
 * Tries to run an external command.
 */
static void
RunExternalCmd(commandT* cmd, bool fork)
{
  if (ResolveExternalCmd(cmd))
    Exec(cmd, fork);
}  /* RunExternalCmd */


/*
 * ResolveExternalCmd
 *
 * arguments:
 *   commandT *cmd: the command to be run
 *
 * returns: bool: whether the given command exists
 *
 * Determines whether the command to be run actually exists.
 */
static bool
ResolveExternalCmd(commandT* cmd)
{
  return FALSE;
} /* ResolveExternalCmd */


/*
 * Exec
 *
 * arguments:
 *   commandT *cmd: the command to be run
 *   bool forceFork: whether to fork
 *
 * returns: none
 *
 * Executes a command.
 */
static void
Exec(commandT* cmd, bool forceFork)
{
} /* Exec */

void Push(pid_t pid,char* js)
{
    bgjobL* newjob = malloc(sizeof(bgjobL));
    newjob->next = bgjobs;
    newjob->pid = pid;
		newjob->bg_js = js;
}

void Pop(bgjobL* popped_job){
//whatever runs Pop must free bgjobL
	if(bgjobs == NULL){
		popped_job = NULL;
	}else{
		popped_job = bgjobs;
		bgjobs = bgjobs->next;
	}

}


void RemoveBgProcess(pid_t pid)
{
    if (!bgjobs)
        perror("No background processes.");
    if (bgjobs->next == NULL) {
        if (bgjobs->pid == pid) {
            free(bgjobs);
            bgjobs = NULL;
        }
        else
            perror("Not a BG Process.");
    }
    else {
        bgjobL* prev = bgjobs;
        bgjobL* jobs = bgjobs->next;
        while (jobs != NULL) {
            if (jobs->pid == pid) {
                prev->next = jobs->next;
                free(jobs);
            }
            else {
                jobs = jobs->next;
                prev = prev->next;
            }
        }
    }
    perror("Not a BG Process.");
}

/*
 * IsBuiltIn
 *
 * arguments:
 *   char *cmd: a command string (e.g. the first token of the command line)
 *
 * returns: bool: TRUE if the command string corresponds to a built-in
 *                command, else FALSE.
 *
 * Checks whether the given string corresponds to a supported built-in
 * command.
 */
int
IsBuiltIn(char* cmd)
{
	if( (strcmp(cmd,"cd") == 0) || (strcmp(cmd,"fg") == 0) || (strcmp(cmd,"jobs") == 0) || (strcmp(cmd,"bg") == 0) ){
			return TRUE;
	}
  return FALSE;
} /* IsBuiltIn */


/*
 * RunBuiltInCmd
 *
 * arguments:
 *   commandT *cmd: the command to be run
 *
 * returns: none
 *
 * Runs a built-in command.
 */
void
RunBuiltInCmd(commandT* cmd)
{
	if(strcmp("cd",cmd->argv[0]) == 0){
			if(chdir(cmd->argv[1]) < 0){
				perror("cd error");	
			}
	}
    if (strcmp("bg", cmd->argv[0]) == 0) {
        if (cmd->argc == 1) {
            if (bgjobs)
                kill(bgjobs->pid, SIGCONT);
        }
        else {
            int id = atoi(cmd->argv[1]);
            if (id < 20) {
                kill(GetPid(id), SIGCONT);
            }
            else {
                kill(id, SIGCONT);
            }
        }
    }
} /* RunBuiltInCmd */


/*
 * CheckJobs
 *
 * arguments: none
 *
 * returns: none
 *
 * Checks the status of running jobs.
 */
void
CheckJobs()
{
} /* CheckJobs */

pid_t GetPid(int jid)
{
    bgjobL* list = bgjobs;
    if (jid < 1) {
        perror("Invalid job id.");
        exit(-1);
    }
    while (jid != 1) {
        if (bgjobs->next) {
            list = bgjobs->next;
            --jid;
        }
        else {
            perror("Invalid job id.");
            exit(-1);
        }
    }
    return list->pid;
}

