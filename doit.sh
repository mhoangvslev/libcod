#!/bin/bash

# ./doit.sh clean
# ./doit.sh cod2_1_0
# ./doit.sh cod2_1_2
# ./doit.sh cod2_1_3

cc="gcc"
options="-I. -m32 -fPIC -Wall -Wno-write-strings"

mysql_link=""
mysql_config=""
mysql_libpath=""
mysql_libpath2=""

MACHINE_TYPE=`uname -m`
if [ ${MACHINE_TYPE} == 'x86_64' ]; then
	mysql_libpath="/usr/lib/i386-linux-gnu/libmysqlclient.so"
	mysql_libpath2="/usr/lib/i386-linux-gnu/libmysqlclient.so.18"
else
	mysql_libpath="/usr/lib/libmysqlclient.so"
	mysql_libpath2="/usr/lib/libmysqlclient.so.18"
fi

if [ "$1" != "clean" ]; then
	if [ -e $mysql_libpath ] || [ -e $mysql_libpath2 ]; then
		mysql_enable="true"
		mysql_config="`mysql_config --cflags --libs`"
	elif [ -d "./vendors/lib" ]; then
		mysql_enable="true"
		mysql_link="-lmysqlclient -L./vendors/lib"
	else
		mysql_enable="false"
		sed -i "/#define COMPILE_MYSQL 1/c\#define COMPILE_MYSQL 0" config.hpp
	fi
fi

if [ "$1" == "clean" ]; then
	echo "##### CLEAN OBJECTS #####"
	rm objects_* -rf
	rm bin -rf
	exit 1

elif [ "$1" == "cod2_1_0" ]; then
	constants="-D COD_VERSION=COD2_1_0"

elif [ "$1" == "cod2_1_2" ]; then
	constants="-D COD_VERSION=COD2_1_2"

elif [ "$1" == "cod2_1_3" ]; then
	constants="-D COD_VERSION=COD2_1_3"

elif [ "$1" == "" ]; then
	echo "##### Please specify a command line option #####"
	exit 0

else
	echo "##### Unrecognized command line option $1 #####"
	exit 0
fi

if [ -f extra/functions.hpp ]; then
	constants+=" -D EXTRA_FUNCTIONS_INC"
fi

if [ -f extra/config.hpp ]; then
	constants+=" -D EXTRA_CONFIG_INC"
fi

if [ -f extra/includes.hpp ]; then
	constants+=" -D EXTRA_INCLUDES_INC"
fi

if [ -f extra/methods.hpp ]; then
	constants+=" -D EXTRA_METHODS_INC"
fi

mkdir -p bin
mkdir -p objects_$1

if [ "$mysql_enable" == "true" ]; then
	echo "##### COMPILE $1 GSC_MYSQL.CPP #####"
	$cc $options $constants -c gsc_mysql.cpp -o objects_$1/gsc_mysql.opp -lmysqlclient -L/usr/lib/mysql
else
	echo "##### WARNING: MYSQL libs not found, MYSQL compilation skipped #####"
fi

echo "##### COMPILE $1 GSC_MEMORY.CPP #####"
$cc $options $constants -c gsc_memory.cpp -o objects_$1/gsc_memory.opp

echo "##### COMPILE $1 CRACKING.CPP #####"
$cc $options $constants -c cracking.cpp -o objects_$1/cracking.opp

echo "##### COMPILE $1 LIBCOD.CPP #####"
$cc $options $constants -c libcod.cpp -o objects_$1/libcod.opp

echo "##### COMPILE $1 GSC.CPP #####"
$cc $options $constants -c gsc.cpp -o objects_$1/gsc.opp

echo "##### COMPILE $1 GSC_PLAYER.CPP #####"
$cc $options $constants -c gsc_player.cpp -o objects_$1/gsc_player.opp

echo "##### COMPILE $1 GSC_UTILS.CPP #####"
$cc $options $constants -c gsc_utils.cpp -o objects_$1/gsc_utils.opp

if [ -d extra ]; then
	echo "##### COMPILE $1 EXTRAS #####"
	cd extra
	for F in *.cpp;
	do
		echo "###### COMPILE $1 EXTRA: $F #####"
		$cc $options $constants -c $F -o ../objects_$1/${F%.cpp}.opp;
	done
	cd ..
fi

echo "##### LINKING lib$1.so #####"
objects="$(ls objects_$1/*.opp)"
$cc -m32 -shared -L/lib32 $mysql_link -o bin/lib$1.so -ldl $objects $mysql_config

if [ "$mysql_enable" == "false" ]; then
	sed -i "/#define COMPILE_MYSQL 0/c\#define COMPILE_MYSQL 1" config.hpp
fi
