/***************************************************************************
 *  Title: Input/Output 
 * -------------------------------------------------------------------------
 *    Purpose: Handles the input and output
 *    Author: Stefan Birrer
 *    Version: $Revision: 1.2 $
 *    Last Modification: $Date: 2009/10/11 04:45:50 $
 *    File: $RCSfile: io.h,v $
 *    Copyright: (C) 2002 by Stefan Birrer
 ***************************************************************************/
/***************************************************************************
 *  ChangeLog:
 * -------------------------------------------------------------------------
 *    $Log: io.h,v $
 *    Revision 1.2  2009/10/11 04:45:50  npb853
 *    Changing the identation of the project to be GNU.
 *
 *    Revision 1.1  2005/10/13 05:24:59  sbirrer
 *    - added the skeleton files
 *
 *    Revision 1.3  2002/10/24 21:32:47  sempi
 *    final release
 *
 *    Revision 1.2  2002/10/23 21:54:27  sempi
 *    beta release
 *
 *    Revision 1.1  2002/10/15 20:20:56  sempi
 *    Milestone 1
 *
 ***************************************************************************/

#ifndef __IO_H__
#define __IO_H__

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
#ifdef __IO_IMPL__
#define EXTERN
#else
#define EXTERN extern
#endif

/************Global Variables*********************************************/

/************Function Prototypes******************************************/

/***********************************************************************
 *  Title: Print a message
 * ---------------------------------------------------------------------
 *    Purpose: Prints a message to standard output.
 *    Input: a string
 *    Output: void
 ***********************************************************************/
EXTERN void
Print(char*);

/***********************************************************************
 *  Title: Print a newline
 * ---------------------------------------------------------------------
 *    Purpose: Prints a newline character to standard output.
 *    Input: void
 *    Output: void
 ***********************************************************************/
EXTERN void
PrintNewline();

/***********************************************************************
 *  Title: Print a error message
 * ---------------------------------------------------------------------
 *    Purpose: Prints a error message according to perror.
 *    Input: a message
 *    Output: void
 ***********************************************************************/
EXTERN void
PrintPError(char*);

/***********************************************************************
 *  Title: Checks whether input is read from stdin
 * ---------------------------------------------------------------------
 *    Purpose: Checks whether th input is actually read from stdin.
 *    Input: void
 *    Output: true if reading
 ***********************************************************************/
EXTERN bool
IsReading();

/***********************************************************************
 *  Title: Read one command line from stdin
 * ---------------------------------------------------------------------
 *    Purpose: Reads one command line from stdin and returns it to the
 *    callee.
 *    Input: pointer to the buffer (will be resized as necessary) & size
 *    Output: void
 ***********************************************************************/
EXTERN void
getCommandLine(char**, int);

/************External Declaration*****************************************/

/**************Definition***************************************************/

#endif /* __IO_H__ */
