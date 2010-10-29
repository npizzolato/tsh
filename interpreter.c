/***************************************************************************
 *  Title: Input 
 * -------------------------------------------------------------------------
 *    Purpose: Handles the input from stdin
 *    Author: Stefan Birrer
 *    Version: $Revision: 1.4 $
 *    Last Modification: $Date: 2009/10/12 20:50:12 $
 *    File: $RCSfile: interpreter.c,v $
 *    Copyright: (C) 2002 by Stefan Birrer
 ***************************************************************************/
/***************************************************************************
 *  ChangeLog:
 * -------------------------------------------------------------------------
 *    $Log: interpreter.c,v $
 *    Revision 1.4  2009/10/12 20:50:12  jot836
 *    Commented tsh C files
 *
 *    Revision 1.3  2009/10/11 04:45:50  npb853
 *    Changing the identation of the project to be GNU.
 *
 *    Revision 1.2  2008/10/10 00:12:09  jot836
 *    JSO added simple command line parser to interpreter.c. It's not pretty
 *    but it works. Handles quoted strings, and preserves backslash behavior
 *    of bash. Also, added simple skeleton code as well as code to create and
 *    free commandT structs given a command line.
 *
 *    Revision 1.1  2005/10/13 05:24:59  sbirrer
 *    - added the skeleton files
 *
 *    Revision 1.4  2002/10/24 21:32:47  sempi
 *    final release
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
#define __INTERPRETER_IMPL__

/************System include***********************************************/
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
//#include "string.h"
#include <string.h>
#include <sys/types.h>
#include <linux/stat.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<fcntl.h>
/************Private include**********************************************/
#include "interpreter.h"
#include "io.h"
#include "runtime.h"

/************Defines and Typedefs*****************************************/
/*  #defines and typedefs should have their names in all caps.
 *  Global variables begin with g. Global constants with k. Local
 *  variables should be in all lower case. When initializing
 *  structures and arrays, line everything up in neat columns.
 */
typedef struct string_l
{
  char* s;
  struct string_l* next;
} stringL;

int BUFSIZE = 512;
int MAXARGS = 100;

/**************Function Prototypes******************************************/

commandT*
getCommand(char* cmdLine);

void
freeCommand(commandT* cmd);

/**************Implementation***********************************************/
int get_path(char* path_list, int* index,char* search_path){
	int start,i;
	i = *index;
	for(start = i;path_list[i] && path_list[i] !=':';i++){
	}
	int len = i-start;
	strncpy(search_path,path_list+start,len);
	search_path[len] = '\0';
	i++;
	return i-start;
}

int
file_stat (char const* name)
{
  struct stat finfo;
  //get current user information
  uid_t uid = geteuid();
  gid_t gid = getegid();
  //lets do this the easier way
  int usr = 0;
  int grp = 0;


  //stat returns -1 on error
  if(stat(name,&finfo)<0){
      return 0;
  }
  //compare user information to file info
  if(finfo.st_uid == uid){
    usr = 1;
  }
  if(finfo.st_gid == gid){
      grp = 1;
  }
  //if the user has permissions, the group has permissions, or everyone has permiossons set exec
  if( (usr && (finfo.st_mode&S_IXUSR && finfo.st_mode&S_IRUSR)) ||
       (grp && (finfo.st_mode&S_IXGRP && finfo.st_mode&S_IRGRP)) ||
       (finfo.st_mode&S_IXOTH && finfo.st_mode&S_IROTH))
  {
  		return 1;    
	}
  

  return 0;
}
int search_single_path(char* cmd,char* path, char** return_path){
	char* full_path;
	int cmd_len = strlen(cmd);
	if(path){
			int path_len = strlen(path);
			full_path = (char *) malloc(2+path_len + cmd_len);	
			strcpy(full_path,path);	
			full_path[path_len] = '/';
			full_path[path_len+1] = '\0';
			strcat(full_path,cmd);
	}else{
			full_path = (char *)malloc(2+cmd_len);
			strcpy(full_path,cmd);
	}
	if(file_stat(full_path)){
			*return_path = malloc(strlen(full_path+2));
			strcpy(*return_path,full_path);
			
	}
	free(full_path);
	return 0;
}

void
search_total_path(char* cmd,char* path_list,char** return_path){
		int i,len,offset=0,found=0;

		len = strlen(path_list);	
		if(cmd){	
		if(cmd && (cmd[0] == '/'  || cmd[0] == '~' || (sizeof(cmd) > 1 && cmd[0] == '.' &&  cmd[1] == '/')) ){
				if(cmd[0] == '/'){
						search_single_path(cmd,NULL,return_path);
				}else if(cmd[0] == '~'){
					char* home_name = malloc(256);
					strcpy(home_name,getenv("HOME"));
					char* path = malloc(2+strlen(home_name) + strlen(cmd));		
					strcpy(path,home_name);			
					strcat(path,cmd+1);
					search_single_path(path,NULL,return_path);
					free(home_name);
					free(path);
				}else if(cmd[0] == '.'){
					char* path_name = malloc(256);
					if(!getcwd(path_name,256)){
						free(path_name);
						path_name = malloc(512);
						if(!getcwd(path_name,512)){
								printf("mad errorz\n");
						}
					}
					char* path = malloc(2+strlen(path_name) + strlen(cmd));		
					strcpy(path,path_name);			
					strcat(path,cmd+1);
					search_single_path(path,NULL,return_path);
					free(path);
					free(path_name);	
				}
		
		}else{
				for(i=0;i<len && !found;i+=offset){
							
					char* search_path = malloc(256);
					offset = get_path(path_list, &i,search_path);
					found = search_single_path(cmd,search_path,return_path);
					free(search_path);
				}
		}
		}
}

char* 
find_command(char* cmd, char* path_list){
	//search_total_path(cmd,path_list);
	return NULL;	
}


/*
 * Interpret
 *
 * arguments:
 *   char *cmdLine: pointer to the command line string
 *
 * returns: explicit path to executable
 *
 * This is the high-level function called by tsh's main to interpret a
 * command line.
 */

void
Interpret(char* cmdLine, char* path_list)
{
	commandT* cmd = getCommand(cmdLine);
	int i,to_pipe=0,to_bg = 0;
	cmd->name = NULL;
  if (cmd->argc <= 0)
    return;
 	if(IsBuiltIn(cmd->argv[0])){
		RunBuiltInCmd(cmd);
	}else{
				
  for (i = 0; cmd->argv[i] != 0; i++)
   {
			if(*(cmd->argv[i]) == '|'){
				to_pipe = i;
			}
   }
	 if(*(cmd->argv[cmd->argc-1]) == '&'){
					to_bg = 1;
					cmd->argv[cmd->argc-1] = 0;
	 }

	 if(!to_pipe){
		search_total_path(cmd->argv[0],path_list,&(cmd->name));
		if((cmd->name)!=NULL){
			if(to_bg){
				RunCmdBg(cmd);
			}
			else{
					RunCmd(cmd);
			}
		}else{
			printf("%s: command not found\n",cmd->argv[0]);
		}
	 }else{
  		commandT* cmd2 = calloc(1,sizeof(commandT) + sizeof(char*) * MAXARGS);
  		cmd2->argv[0] = 0;
  		cmd2->name = 0;
  		cmd2->argc = 0;
			free(cmd->argv[to_pipe]);
			cmd->argv[to_pipe] = 0;
			search_total_path(cmd->argv[0],path_list,&(cmd->name));
			search_total_path(cmd->argv[to_pipe+1],path_list,&(cmd2->name));
			if(cmd2->name != NULL){
				int j,p=to_pipe+1;
				for(j=0;cmd->argv[p]!=0 || cmd->argv[p]!=NULL;j++){
					//cmd2->argv[j] = malloc(2+strlen(cmd->argv[p]));		
					cmd2->argc++;
					cmd2->argv[j] = cmd->argv[p];
					strcpy( cmd2->argv[j], cmd->argv[p]);	
					//free(cmd->argv[p]);
					cmd->argv[p] = 0;
					p++;
				}

				cmd2->argv[j+1] = 0;
				RunCmdPipe(cmd,cmd2);
				//MAKE SURE TO FREE COMMAND
				freeCommand(cmd2);
			}else{
					printf("%s: command not found\n",cmd->argv[to_pipe+1]);
			}
			
	 }

	}
  freeCommand(cmd);
} /* Interpret */


/*
 * getCommand
 *
 * arguments:
 *   char *cmdLine: pointer to the command line string
 *
 * returns: commandT*: pointer to the commandT struct generated by
 *                     parsing the cmdLine string
 *
 * This parses the command line string, and returns a commandT struct,
 * as defined in runtime.h.  You must free the memory used by commandT
 * using the freeCommand function after you are finished.
 *
 * This function tokenizes the input, preserving quoted strings. It
 * supports escaping quotes and the escape character, '\'.
 */
commandT*
getCommand(char* cmdLine)
{
  commandT* cmd = malloc(sizeof(commandT) + sizeof(char*) * MAXARGS);
  cmd->argv[0] = 0;
  cmd->name = 0;
  cmd->argc = 0;

  int i, inArg = 0;
  char quote = 0;
  char escape = 0;

  // Set up the initial empty argument
  char* tmp = malloc(sizeof(char*) * BUFSIZE);
  int tmpLen = 0;
  tmp[0] = 0;

  //printf("parsing:%s\n", cmdLine);

  for (i = 0; cmdLine[i] != 0; i++)
    {
      //printf("\tindex %d, char %c\n", i, cmdLine[i]);

      // Check for whitespace
      if (cmdLine[i] == ' ')
        {
          if (inArg == 0)
            continue;
          if (quote == 0)
            {
              // End of an argument
              //printf("\t\tend of argument %d, got:%s\n", cmd.argc, tmp);
              cmd->argv[cmd->argc] = malloc(sizeof(char) * (tmpLen + 1));
              strcpy(cmd->argv[cmd->argc], tmp);

              inArg = 0;
              tmp[0] = 0;
              tmpLen = 0;
              cmd->argc++;
              cmd->argv[cmd->argc] = 0;
              continue;
            }
        }

      // If we get here, we're in text or a quoted string
      inArg = 1;

      // Start or end quoting.
      if (cmdLine[i] == '\'' || cmdLine[i] == '"')
        {
          if (escape != 0 && quote != 0 && cmdLine[i] == quote)
            {
              // Escaped quote. Add it to the argument.
              tmp[tmpLen++] = cmdLine[i];
              tmp[tmpLen] = 0;
              escape = 0;
              continue;
            }

          if (quote == 0)
            {
              //printf("\t\tstarting quote around %c\n", cmdLine[i]);
              quote = cmdLine[i];
              continue;
            }
          else
            {
              if (cmdLine[i] == quote)
                {
                  //printf("\t\tfound end quote %c\n", quote);
                  quote = 0;
                  continue;
                }
            }
        }

      // Handle escape character repeat
      if (cmdLine[i] == '\\' && escape == '\\')
        {
          escape = 0;
          tmp[tmpLen++] = '\\';
          tmp[tmpLen] = 0;
          continue;
        }

      // Handle single escape character followed by a non-backslash or quote character
      if (escape == '\\')
        {
          if (quote != 0)
            {
              tmp[tmpLen++] = '\\';
              tmp[tmpLen] = 0;
            }
          escape = 0;
        }

      // Set the escape flag if we have a new escape character sequence.
      if (cmdLine[i] == '\\')
        {
          escape = '\\';
          continue;
        }

      tmp[tmpLen++] = cmdLine[i];
      tmp[tmpLen] = 0;
    }
  // End the final argument, if any.
  if (tmpLen > 0)
    {
      //printf("\t\tend of argument %d, got:%s\n", cmd.argc, tmp);
      cmd->argv[cmd->argc] = malloc(sizeof(char) * (tmpLen + 1));
      strcpy(cmd->argv[cmd->argc], tmp);

      inArg = 0;
      tmp[0] = 0;
      tmpLen = 0;
      cmd->argc++;
      cmd->argv[cmd->argc] = 0;
    }

  free(tmp);

  cmd->name = cmd->argv[0];

  return cmd;
} /* getCommand */


/*
 * freeCommand
 *
 * arguments:
 *   commandT *cmd: pointer to the commandT struct to be freed
 *
 * returns: none
 *
 * This function frees all the memory associated with the given
 * commandT struct, before freeing the struct's memory itself.
 */
void
freeCommand(commandT* cmd)
{
  int i;
	free(cmd->name);
  for (i = 0; i<cmd->argc && cmd->argv[i] != 0 && cmd->argv[i] !=NULL; i++)
    {
      free(cmd->argv[i]);
      cmd->argv[i] = 0;
    }
  free(cmd);
} /* freeCommand */
