/***************************************************************************
 *  Title: Runtime environment 
 * -------------------------------------------------------------------------
 *    Purpose: Runs commands
 *    Author: Stefan Birrer
 *    Version: $Revision: 1.2 $
 *    Last Modification: $Date: 2009/10/11 04:45:50 $
 *    File: $RCSfile: runtime.h,v $
 *    Copyright: (C) 2002 by Stefan Birrer
 ***************************************************************************/
/***************************************************************************
 *  ChangeLog:
 * -------------------------------------------------------------------------
 *    $Log: runtime.h,v $
 *    Revision 1.2  2009/10/11 04:45:50  npb853
 *    Changing the identation of the project to be GNU.
 *
 *    Revision 1.1  2005/10/13 05:24:59  sbirrer
 *    - added the skeleton files
 *
 *    Revision 1.3  2002/10/23 21:54:27  sempi
 *    beta release
 *
 *    Revision 1.2  2002/10/21 04:47:05  sempi
 *    Milestone 2 beta
 *
 *    Revision 1.1  2002/10/15 20:20:56  sempi
 *    Milestone 1
 *
 ***************************************************************************/

#ifndef __RUNTIME_H__
#define __RUNTIME_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/************System include***********************************************/

/************Private include**********************************************/

/************Defines and Typedefs*****************************************/
/*  #defines and typedefs should have their names in all caps.
 *  Global variables begin with g. Global constants with k. Local
 *  variables should be in all lower case. When initializing
 *  structures and arrays, line everything up in neat columns.
 */

#undef EXTERN
#ifdef __RUNTIME_IMPL__
#define EXTERN
#define VAREXTERN(x, y) x = y;
#else
#define EXTERN extern
#define VAREXTERN(x, y) extern x;
#endif

typedef struct command_t
{
  char* name;
  int argc;
  char* argv[];
} commandT;

/************Global Variables*********************************************/

/***********************************************************************
 *  Title: Force a program exit
 * ---------------------------------------------------------------------
 *    Purpose: Signals that a program exit is required
 ***********************************************************************/
VAREXTERN(bool forceExit, FALSE)
;

/************Function Prototypes******************************************/

/***********************************************************************
 *  Title: Runs a command
 * ---------------------------------------------------------------------
 *    Purpose: Runs a command.
 *    Input: a command structure
 *    Output: void
 ***********************************************************************/
EXTERN void
RunCmd(commandT*);

/***********************************************************************
 *  Title: Runs a command in background
 * ---------------------------------------------------------------------
 *    Purpose: Runs a command in background.
 *    Input: a command structure
 *    Output: void
 ***********************************************************************/
EXTERN void
RunCmdBg(commandT*);

/***********************************************************************
 *  Title: Runs two command with a pipe
 * ---------------------------------------------------------------------
 *    Purpose: Runs two command connected with a pipe.
 *    Input: two command structure
 *    Output: void
 ***********************************************************************/
EXTERN void
RunCmdPipe(commandT*, commandT*);

/***********************************************************************
 *  Title: Runs two command with output redirection
 * ---------------------------------------------------------------------
 *    Purpose: Runs a command and redirects the output to a file.
 *    Input: a command structure structure and a file name
 *    Output: void
 ***********************************************************************/
EXTERN void
RunCmdRedirOut(commandT*, char*);

/***********************************************************************
 *  Title: Runs two command with input redirection
 * ---------------------------------------------------------------------
 *    Purpose: Runs a command and redirects the input to a file.
 *    Input: a command structure structure and a file name
 *    Output: void
 ***********************************************************************/
EXTERN void
RunCmdRedirIn(commandT*, char*);

/***********************************************************************
 *  Title: Stop the foreground process
 * ---------------------------------------------------------------------
 *    Purpose: Stops the current foreground process if there is any.
 *    Input: void
 *    Output: void
 ***********************************************************************/
EXTERN void
StopFgProc();
/***********************************************************************
 *  Title: Create a command structure
 * ---------------------------------------------------------------------
 *    Purpose: Creates a command structure.
 *    Input: the number of arguments
 *    Output: the command structure
 ***********************************************************************/
EXTERN commandT*
CreateCmdT(int);

/***********************************************************************
 *  Title: Release a command structure
 * ---------------------------------------------------------------------
 *    Purpose: Frees the allocated memory of a command structure.
 *    Input: the command structure
 *    Output: void
 ***********************************************************************/
EXTERN void
ReleaseCmdT(commandT**);

/***********************************************************************
 *  Title: Get the current working directory
 * ---------------------------------------------------------------------
 *    Purpose: Gets the current working directory.
 *    Input: void
 *    Output: a string containing the current working directory
 ***********************************************************************/
EXTERN char*
getCurrentWorkingDir();

/***********************************************************************
 *  Title: Get user name
 * ---------------------------------------------------------------------
 *    Purpose: Gets user name logged in on the controlling terminal.
 *    Input: void
 *    Output: a string containing the user name
 ***********************************************************************/
EXTERN char*
getLogin();

/***********************************************************************
 *  Title: Check the jobs
 * ---------------------------------------------------------------------
 *    Purpose: Checks the status of the background jobs.
 *    Input: void
 *    Output: void
 ***********************************************************************/
EXTERN void
CheckJobs();

/************External Declaration*****************************************/

/**************Definition***************************************************/

#endif /* __RUNTIME_H__ */
