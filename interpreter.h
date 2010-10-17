/***************************************************************************
 *  Title: Interpreter 
 * -------------------------------------------------------------------------
 *    Purpose: Interprets a command line
 *    Author: Stefan Birrer
 *    Version: $Revision: 1.2 $
 *    Last Modification: $Date: 2009/10/11 04:45:50 $
 *    File: $RCSfile: interpreter.h,v $
 *    Copyright: (C) 2002 by Stefan Birrer
 ***************************************************************************/
/***************************************************************************
 *  ChangeLog:
 * -------------------------------------------------------------------------
 *    $Log: interpreter.h,v $
 *    Revision 1.2  2009/10/11 04:45:50  npb853
 *    Changing the identation of the project to be GNU.
 *
 *    Revision 1.1  2005/10/13 05:24:59  sbirrer
 *    - added the skeleton files
 *
 *    Revision 1.1  2002/10/15 20:20:56  sempi
 *    Milestone 1
 *
 ***************************************************************************/

#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

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
#ifdef __INTERPRETER_IMPL__
#define EXTERN
#else
#define EXTERN extern
#endif

/************Global Variables*********************************************/

/************Function Prototypes******************************************/

/***********************************************************************
 *  Title: Interprets a command line
 * ---------------------------------------------------------------------
 *    Purpose: Interprets a command line and executes the desired
 *    programs
 *    Input: a command line
 *    Output: void
 ***********************************************************************/
EXTERN void
Interpret(char*);

/************External Declaration*****************************************/

/**************Definition***************************************************/

#endif /* __INTERPRETER_H__ */
