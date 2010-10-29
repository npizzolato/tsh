###############################################################################
#
# File:         Makefile
# RCS:          $Id: Makefile,v 1.3 2009/10/12 19:10:53 jot836 Exp $
# Description:  Guess
# Author:       Fabian E. Bustamante
#               Northwestern Systems Research Group
#               Department of Computer Science
#               Northwestern University
# Created:      Fri Sep 12, 2003 at 15:56:30
# Modified:     Wed Sep 24, 2003 at 18:31:43 fabianb@cs.northwestern.edu
# Language:     Makefile
# Package:      N/A
# Status:       Experimental (Do Not Distribute)
#
# (C) Copyright 2003, Northwestern University, all rights reserved.
#
###############################################################################

# handin info
TEAM = 'njp821_mas384'
VERSION = `date +%Y%m%d%H%M%S`
PROJ = tsh

CC = gcc
MV = mv
CP = cp
RM = rm
MKDIR = mkdir
TAR = tar cvf
COMPRESS = gzip
CFLAGS = -g -Wall -D HAVE_CONFIG_H

DELIVERY = Makefile *.h *.c tsh.1
PROGS = tsh
SRCS = interpreter.c io.c runtime.c tsh.c 
OBJS = ${SRCS:.c=.o}

all: ${PROGS}

test-reg: handin
	HANDIN=`pwd`/${TEAM}-${VERSION}-${PROJ}.tar.gz;\
	cd testsuite;\
	sh ./run_testcase.sh $${HANDIN};

handin: cleanAll
	${TAR} ${TEAM}-${VERSION}-${PROJ}.tar ${DELIVERY}
	${COMPRESS} ${TEAM}-${VERSION}-${PROJ}.tar

.o:
	${CC} *.c

tsh: ${OBJS}
	${CC} -o $@ ${OBJS}

clean:
	${RM} -f *.o *~

cleanAll: clean
	${RM} -f ${PROGS} ${TEAM}-${VERSION}-${PROJ}.tar.gz
