#!/bin/sh

source config.test;

if [[ ! "$#" -eq 1 ]]; then
	echo -e "usage: $0 targz_file";
	exit 1;
fi;

TARGZ=$1;
TC_DIR=`pwd`;
SDRIVER="sdriver.pl";
SRCDIR=`mktemp -d /tmp/cs343.tests.XXXXXX`;
TMP=`mktemp -d /tmp/cs343.tests.XXXXXX`;
OUTPUT=`mktemp -d /tmp/cs343.tests.XXXXXX`;
chmod go-rwx ${SRCDIR} ${TMP} ${OUTPUT} || exit 1;

function cleanUp()
{
	rm -Rf ${SRCDIR} ${TMP} ${OUTPUT};
}

echo "Testing ${TARGZ}";
echo;

# Untar sources
echo "UNTAR";
cd ${SRCDIR} || { cleanUp; exit 1; }
tar xvfz ${TARGZ} || { cleanUp; exit 1; }

echo;

if [[ `ls *.c |wc -w` -eq 0 ]]; then
    echo "error: no source files (*.c) found";
	echo "Please follow the submission instructions carefully.";
	cleanUp;
    exit 1;
fi;

if [ ! -f Makefile -a ! -f makefile ]; then
	echo "warning: Makefile is missing";
	echo;
fi;

# Setup the environment 
cd ${TMP} || { cleanUp; exit 1; }
sh ${TC_DIR}/setup.sh;
cp ${TC_DIR}/${SDRIVER} .;
cp ${TC_DIR}/${ORIG} .;
gcc ${TC_DIR}/myspin.c -o myspin

# Compile the code
echo "COMPILE"
for SRC in ${SRCDIR}/*.c; do
	echo "compile $SRC";
	gcc -g -Wall -O2 -D HAVE_CONFIG_H -c ${SRC} >> ${OUTPUT}/gcc.output 2>&1;
	echo "-----" >> ${OUTPUT}/gcc.output;
done
echo "LINKING";
gcc	*.o -o ${TMP}/${BIN} >> ${OUTPUT}/gcc.output 2>&1;
WARNING=`grep -c warning ${OUTPUT}/gcc.output`
ERROR=`grep -c error ${OUTPUT}/gcc.output`

echo "${WARNING} warning(s) found while compiling"
echo "${ERROR} error(s) found while compiling"

if [ ${WARNING} -gt 0 -o ${ERROR} -gt 0 ]; then
	echo;
	echo "GCC OUTPUT";
	cat ${OUTPUT}/gcc.output;
fi;

if [ ! -x ${TMP}/${BIN} ]; then
	echo "error: failed to create executable";
	cat ${OUTPUT}/gcc.output;
	cleanUp;
	exit 1;
fi;

echo;

# Run test cases
echo "RUN BASIC TEST CASES";
TC_PASSED=0;
for tc in ${BASIC_TESTS}; do
	ARGS="";
	if [[ -f ${TC_DIR}/${tc}.arg ]]; then
		ARGS="${ARGS} $(<${TC_DIR}/${tc}.arg)";
	fi
	if [[ -f ${TC_DIR}/${tc}.out ]]; then
		cp ${TC_DIR}/${tc}.out ${OUTPUT}/${tc}.orig;
	else
		if [[ -f ${TC_DIR}/${tc}.in ]]; then
			./${SDRIVER} -t ${TC_DIR}/${tc}.in -s ./${ORIG} > ${OUTPUT}/${tc}.orig 2>&1;
#                        cp ${OUTPUT}/${tc}.orig ${TC_DIR}/${tc}.out;
		else
			${TC_DIR}/${ORIG} ${ARGS} > ${OUTPUT}/${tc}.orig 2>&1;		
		fi
	fi

	if [[ -f ${TC_DIR}/${tc}.in ]]; then
		./${SDRIVER} -t ${TC_DIR}/${tc}.in -s ./${BIN} > ${OUTPUT}/${tc}.bin 2>&1;
	else
		./${BIN} ${ARGS} > ${OUTPUT}/${tc}.bin 2>&1;
	fi

	${DIFF} ${OUTPUT}/${tc}.orig ${OUTPUT}/${tc}.bin >/dev/null;
	if [[ "$?" -eq 0 ]]; then
		echo "${tc}: PASS";
		((TC_PASSED++));
	else
		echo "${tc}: FAILED";
		if [[ -f ${TC_DIR}/${tc}.arg ]]; then
			echo "Args:";
			cat ${TC_DIR}/${tc}.arg;
		fi
		if [[ -f ${TC_DIR}/${tc}.in ]]; then
			echo "Input:";
			cat ${TC_DIR}/${tc}.in;
		fi
		echo "-- HOW IT SHOULD BE ------------------------------------------------------------ YOUR PROGRAM --------------------------------------------------------------";
		diff --side-by-side -W 160 ${OUTPUT}/${tc}.orig ${OUTPUT}/${tc}.bin;
		echo "------------------------------------------------------------------------------------------------------------------------------------------------------------";
	fi
done;

echo;
echo "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=";
echo "${TC_PASSED} basic test cases passed";
echo;

# Run extra credit test cases
echo "RUN EXTRA CREDIT TEST CASES";
ETC_PASSED=0;
for tc in ${EXTRA_TESTS}; do
	ARGS="";
	if [[ -f ${TC_DIR}/${tc}.arg ]]; then
		ARGS="${ARGS} $(<${TC_DIR}/${tc}.arg)";
	fi
	if [[ -f ${TC_DIR}/${tc}.out ]]; then
		cp ${TC_DIR}/${tc}.out ${OUTPUT}/${tc}.orig;
	else
		if [[ -f ${TC_DIR}/${tc}.in ]]; then
                        ./${SDRIVER} -t ${TC_DIR}/${tc}.in -s ${TC_DIR}/${ORIG} > ${OUTPUT}/${tc}.orig 2>&1;
#                        cp ${OUTPUT}/${tc}.orig ${TC_DIR}/${tc}.out;
		else
			${TC_DIR}/${ORIG} ${ARGS} > ${OUTPUT}/${tc}.orig 2>&1;		
		fi
	fi

	if [[ -f ${TC_DIR}/${tc}.in ]]; then
                ./${SDRIVER} -t ${TC_DIR}/${tc}.in -s ./${BIN} > ${OUTPUT}/${tc}.bin 2>&1;
	else
		./${BIN} ${ARGS} > ${OUTPUT}/${tc}.bin 2>&1;
	fi

	${DIFF} ${OUTPUT}/${tc}.orig ${OUTPUT}/${tc}.bin >/dev/null;
	if [[ "$?" -eq 0 ]]; then
		echo "${tc}: PASS";
		((ETC_PASSED++));
	else
		echo "${tc}: FAILED";
		if [[ -f ${TC_DIR}/${tc}.arg ]]; then
			echo "Args:";
			cat ${TC_DIR}/${tc}.arg;
		fi
		if [[ -f ${TC_DIR}/${tc}.in ]]; then
			echo "Input:";
			cat ${TC_DIR}/${tc}.in;
		fi
		echo "-- HOW IT SHOULD BE ------------------------------------------------------------ YOUR PROGRAM --------------------------------------------------------------";
		diff --side-by-side -W 160 ${OUTPUT}/${tc}.orig ${OUTPUT}/${tc}.bin;
		echo "------------------------------------------------------------------------------------------------------------------------------------------------------------";
	fi
done;

echo;
echo "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=";
echo "${ETC_PASSED} extra credit test cases passed";
echo;

# Check for memory leaks
echo "CHECK FOR MEMORY LEAKS";
if [[ `which valgrind 2>&1 | grep -c "no valgrind"` -gt 0 ]]; then
	echo "error: valgrind not in PATH";
	echo "error: skip memory leak check";
else
	POS_LEAKS=0;
	DEF_LEAKS=0;
	STILL_REACHABLE=0;

	for tc in ${MEMORY_TESTS}; do
		ARGS="";
	    if [[ -f ${TC_DIR}/${tc}.arg ]]; then
    	    ARGS="${ARGS} $(<${TC_DIR}/${tc}.arg)";
		fi

		if [[ -f ${TC_DIR}/${tc}.in ]]; then
			valgrind -v --tool=memcheck --show-reachable=yes --leak-check=yes ./${BIN} < ${TC_DIR}/${tc}.in > ${OUTPUT}/${tc}.valgrind 2>&1;
		else
			valgrind -v --tool=memcheck --show-reachable=yes --leak-check=yes ./${BIN} ${ARGS} > ${OUTPUT}/${tc}.valgrind 2>&1;
	    fi
		
		if [[ -n ${VERBOSE} ]]; then
			cat ${OUTPUT}/${tc}.valgrind;
		fi;

		NOCHECK=`grep -c "no leaks are possible" ${OUTPUT}/${tc}.valgrind`;
		if [[ ${NOCHECK} -eq 0 ]]; then
			leaks=`grep "possibly lost:" ${OUTPUT}/${tc}.valgrind | sed "s/.*bytes in[[:space:]]*//" |sed "s/[[:space:]]*blocks.*//";`
			if [[ ${leaks} -gt ${POS_LEAKS} ]]; then
				POS_LEAKS=${leaks};
			fi;
			leaks=`grep "definitely lost:" ${OUTPUT}/${tc}.valgrind | sed "s/.*bytes in[[:space:]]*//" |sed "s/[[:space:]]*blocks.*//";`
			if [[ ${leaks} -gt ${DEF_LEAKS} ]]; then
				DEF_LEAKS=${leaks};
			fi;
			leaks=`grep "still reachable:" ${OUTPUT}/${tc}.valgrind | sed "s/.*bytes in[[:space:]]*//" |sed "s/[[:space:]]blocks.*//";`
			if [[ ${leaks} -gt ${DEF_LEAKS} ]]; then
				STILL_REACHABLE=${leaks};
			fi;
		fi;
	done;

	echo "${POS_LEAKS} possible leaks";
	echo "${DEF_LEAKS} leaks";
	echo "${STILL_REACHABLE} still reachable";
fi;

	
#Clean up
cleanUp;
