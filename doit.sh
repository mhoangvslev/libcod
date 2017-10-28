#!/bin/bash

# ./doit.sh clean
# ./doit.sh cod2_1_0
# ./doit.sh cod2_1_2
# ./doit.sh cod2_1_3

cc="g++"
options="-I. -m32 -fPIC -Wall -Wno-write-strings"

mysql_variant=0
pthread_link=""
sqlite_found="false"
sqlite_libpath=""
sqlite_link=""

if [ "$1" != "clean" ]; then
	read -rsp $'\nChoose Your MySQL variant:\n
	0. MySQL disabled. (default)\n
	1. Default MySQL variant: A classic MySQL implentation
	made by kung and izno.
	Multiple connections, multiple threads,
	good for servers that use
	remote MySQL sessions, IRC stuff, and etc.\n
	2. VoroN\'s MySQL variant. My own MySQL implentation.
	Native callbacks, native arguments,
	single connection, single thread,
	good for local MySQL session,
	less cpu usage, less memory usage.\n
	Press a key to continue...\n' -n1 key

	if [ "$key" = '1' ]; then
		mysql_variant=1
		sed -i "/#define COMPILE_MYSQL_DEFAULT 0/c\\#define COMPILE_MYSQL_DEFAULT 1" config.hpp
		if [ -d "./vendors/lib" ]; then
			mysql_link="-lmysqlclient -L./vendors/lib"
			export LD_LIBRARY_PATH_32="./vendors/lib"
		else
			mysql_link="-lmysqlclient -L/usr/lib/mysql"
		fi
	elif [ "$key" = '2' ]; then
		mysql_variant=2
		sed -i "/#define COMPILE_MYSQL_VORON 0/c\\#define COMPILE_MYSQL_VORON 1" config.hpp
		if [ -d "./vendors/lib" ]; then
			mysql_link="-lmysqlclient -L./vendors/lib"
			export LD_LIBRARY_PATH_32="./vendors/lib"
		else
			mysql_link="-lmysqlclient -L/usr/lib/mysql"
		fi
	else
		mysql_link=""
		mysql_variant=0
	fi

	MACHINE_TYPE=$(uname -m)
	if [ ${MACHINE_TYPE} == 'x86_64' ]; then
		sqlite_libpath="/usr/lib32/libsqlite3.so"
	else
		sqlite_libpath="/usr/lib/libsqlite3.so"
	fi

	if [ -e $sqlite_libpath ]; then
		sqlite_found="true"
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
mkdir -p objects_"$1"

echo "##### COMPILE $1 CRACKING.CPP #####"
$cc $options $constants -c cracking.cpp -o objects_"$1"/cracking.opp

echo "##### COMPILE $1 GSC.CPP #####"
$cc $options $constants -c gsc.cpp -o objects_"$1"/gsc.opp

if [ "$(< config.hpp grep '#define COMPILE_BOTS' | grep -o '[0-9]')" == "1" ]; then
	echo "##### COMPILE $1 GSC_BOTS.CPP #####"
	$cc $options $constants -c gsc_bots.cpp -o objects_"$1"/gsc_bots.opp
fi

if [ "$(< config.hpp grep '#define COMPILE_EXEC' | grep -o '[0-9]')" == "1" ]; then
	echo "##### COMPILE $1 GSC_EXEC.CPP #####"
	$cc $options $constants -c gsc_exec.cpp -o objects_"$1"/gsc_exec.opp
	pthread_link="-lpthread"
fi

if [ "$(< config.hpp grep '#define COMPILE_MEMORY' | grep -o '[0-9]')" == "1" ]; then
	echo "##### COMPILE $1 GSC_MEMORY.CPP #####"
	$cc $options $constants -c gsc_memory.cpp -o objects_"$1"/gsc_memory.opp
fi

if [ "$(< config.hpp grep '#define COMPILE_MYSQL_DEFAULT' | grep -o '[0-9]')" == "1" ]; then
	echo "##### COMPILE $1 GSC_MYSQL.CPP #####"
	$cc $options $constants -c gsc_mysql.cpp -o objects_"$1"/gsc_mysql.opp
	pthread_link="-lpthread"
fi

if [ "$(< config.hpp grep '#define COMPILE_MYSQL_VORON' | grep -o '[0-9]')" == "1" ]; then
	echo "##### COMPILE $1 GSC_MYSQL_VORON.CPP #####"
	$cc $options $constants -c gsc_mysql_voron.cpp -o objects_"$1"/gsc_mysql_voron.opp
	pthread_link="-lpthread"
fi

if [ "$(< config.hpp grep '#define COMPILE_PLAYER' | grep -o '[0-9]')" == "1" ]; then
	echo "##### COMPILE $1 GSC_PLAYER.CPP #####"
	$cc $options $constants -c gsc_player.cpp -o objects_"$1"/gsc_player.opp
fi

if [ $sqlite_found == "true" ]; then
	if [ "$(< config.hpp grep '#define COMPILE_SQLITE' | grep -o '[0-9]')" == "1" ]; then
		echo "##### COMPILE $1 GSC_SQLITE.CPP #####"
		$cc $options $constants -c gsc_sqlite.cpp -o objects_"$1"/gsc_sqlite.opp
		sqlite_link="-lsqlite3"
	fi
else
	echo "##### WARNING: SQLite lib not found, SQLite compilation skipped #####"
fi

if [ "$(< config.hpp grep '#define COMPILE_UTILS' | grep -o '[0-9]')" == "1" ]; then
	echo "##### COMPILE $1 GSC_UTILS.CPP #####"
	$cc $options $constants -c gsc_utils.cpp -o objects_"$1"/gsc_utils.opp
fi

if [ "$(< config.hpp grep '#define COMPILE_WEAPONS' | grep -o '[0-9]')" == "1" ]; then
	echo "##### COMPILE $1 GSC_WEAPONS.CPP #####"
	$cc $options $constants -c gsc_weapons.cpp -o objects_"$1"/gsc_weapons.opp
fi

if [ -d extra ]; then
	echo "##### COMPILE $1 EXTRAS #####"
	cd extra
	for F in *.cpp;
	do
		echo "###### COMPILE $1 EXTRA: $F #####"
		$cc $options $constants -c "$F" -o ../objects_"$1"/"${F%.cpp}".opp;
	done
	cd ..
fi

echo "##### COMPILE $1 LIBCOD.CPP #####"
$cc $options $constants -c libcod.cpp -o objects_"$1"/libcod.opp

echo "##### LINKING lib$1.so #####"
objects="$(ls objects_$1/*.opp)"
$cc -m32 -shared -L/lib32 -o bin/lib"$1".so -ldl $objects $pthread_link $mysql_link $sqlite_link
rm objects_"$1" -r

if [ $mysql_variant -gt 0 ]; then
	sed -i "/#define COMPILE_MYSQL_DEFAULT 1/c\\#define COMPILE_MYSQL_DEFAULT 0" config.hpp
	sed -i "/#define COMPILE_MYSQL_VORON 1/c\\#define COMPILE_MYSQL_VORON 0" config.hpp
fi
