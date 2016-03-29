#!/bin/bash

# ./doit.sh tar
# ./doit.sh base
# ./doit.sh clean
# ./doit.sh tcc
# ./doit.sh car
# ./doit.sh cod1_1_5
# ./doit.sh cod2_1_0
# ./doit.sh cod2_1_2
# ./doit.sh cod2_1_3
# ./doit.sh cod4_1_7
# ./doit.sh cod4_1_7_l

# clang warns about everything
cc="gcc"

options="-I. -m32 -fPIC -Wno-write-strings"
	
tmp="/$XDG_DATA_HOME/q3rally/q3rallysa/build";
objects_car="$tmp/q_shared.o $tmp/q_math.o $tmp/com_printf.o $tmp/bg_wheel_forces.o $tmp/bg_pmove.o $tmp/bg_physics.o $tmp/bg_misc.o"

mysql_link=""
mysql_config=""
mysql_libpath=""

MACHINE_TYPE=`uname -m`
if [ ${MACHINE_TYPE} == 'x86_64' ]; then
	mysql_libpath="/usr/lib/i386-linux-gnu/libmysqlclient.so.18"
else
	mysql_libpath="/usr/lib/libmysqlclient.so.18"
fi

if [ "$1" != "tar" ] && [ "$1" != "clean" ] && [ "$1" != "tcc" ] && [ "$1" != "car" ]; then
	if [ -e $mysql_libpath ]; then
		mysql_enable="true"
		mysql_config="`mysql_config --cflags --libs`"
	elif [ -d "./vendors/lib" ]; then
		mysql_enable="true"
		mysql_link="-lmysqlclient -L./vendors/lib"
	else
		mysql_enable="false"
		sed -i "/#define COMPILE_MYSQL 1/c\#define COMPILE_MYSQL 0" config.hpp
		sed -i "/#define COMPILE_MYSQL_TESTS 1/c\#define COMPILE_MYSQL_TESTS 0" config.hpp
	fi
fi

# if dir $java_jdk exists: add java support to libcod
# download url: https://jdk8.java.net/download.html
java_jdk="/root/helper/openjdk8"
java_lib=""
java_header=""
java_enable="false"

# when the JDK is not found, force it to be off
if [ ! -d $java_jdk ]; then
	java_enable="false"
fi

if [ "$java_enable" == "true" ]; then
	java_lib="-ljvm -L$java_jdk/jre/lib/i386/server/"
	java_header="-I$java_jdk/include/ -I$java_jdk/include/linux/"
	options="$options -DIS_JAVA_ENABLED"
fi

if [ "$1" == "tar" ]; then
	echo "##### TAR LIBCOD #####"
	
	rm libcod.tar -rf
	tar -cf  libcod.tar plugins
	tar -rf  libcod.tar *.c
	tar -rf  libcod.tar *.cpp
	tar -rf  libcod.tar *.hpp
	tar -rf  libcod.tar doit.sh

	echo "libcod.tar created: $?"
	exit 1

elif [ "$1" == "base" ]; then
	mkdir -p objects_$1
	echo "##### COMPILE GSC_ASTAR.CPP #####"
	$cc $options -c gsc_astar.cpp -o objects_$1/gsc_astar.opp
	if [ "$mysql_enable" == "true" ]; then
		echo "##### COMPILE GSC_MYSQL.CPP #####"
		$cc $options -c gsc_mysql.cpp -o objects_$1/gsc_mysql.opp -lmysqlclient -L/usr/lib/mysql
	else
		echo "##### WARNING: MYSQL libs not found, MYSQL compilation skipped #####"
	fi
	echo "##### COMPILE SERVER.C #####"
	$cc $options -c server.c -o objects_$1/server.opp -D SERVER_PORT=8000
	echo "##### COMPILE GSC_MEMORY.CPP #####"
	$cc $options -c gsc_memory.cpp -o objects_$1/gsc_memory.opp
	echo "##### COMPILE CRACKING.CPP #####"
	$cc $options -c cracking.cpp -o objects_$1/cracking.opp
	echo "##### COMPILE GSC_MATH.CPP #####"
	$cc $options -o objects_$1/gsc_math.opp -c gsc_math.cpp
	echo "##### COMPILE JAVA_EMBED.C #####"
	if [ "$java_enable" == "true" ]; then
		$cc $options -o objects_$1/java_embed.opp -c java_embed.c $java_header
	else
		echo "##### WARNING: Skipped java_embed.c because /root/helper/openjdk8 does not exist #####"
	fi
	if [ "$mysql_enable" == "false" ]; then
		sed -i "/#define COMPILE_MYSQL 0/c\#define COMPILE_MYSQL 1" config.hpp
		sed -i "/#define COMPILE_MYSQL_TESTS 0/c\#define COMPILE_MYSQL_TESTS 1" config.hpp
	fi
	exit 1

elif [ "$1" == "clean" ]; then
	echo "##### CLEAN OBJECTS #####"
	rm objects_* -rf
	rm bin -rf
	rm libcod.tar -rf
	exit 1

elif [ "$1" == "tcc" ]; then
	mkdir -p objects_$1

	echo "##### COMPILE GSC_TCC.cpp #####"
	$cc $options -c gsc_tcc.cpp -o objects_$1/gsc_tcc.opp
	exit 1

elif [ "$1" == "car" ]; then
	mkdir -p objects_$1

	echo "##### COMPILE GSC_CAR.CPP #####"
	$cc $options -c gsc_car.cpp -o objects_$1/gsc_car.opp -I/$XDG_DATA_HOME/q3rally/q3rallysa/
	exit 1

elif [ "$1" == "cod1_1_5" ]; then
	constants="-D COD_VERSION=COD1_1_5"

elif [ "$1" == "cod2_1_0" ]; then
	constants="-D COD2_VERSION=COD2_VERSION_1_0 -D COD_VERSION=COD2_1_0"

elif [ "$1" == "cod2_1_2" ]; then
	constants="-D COD2_VERSION=COD2_VERSION_1_2 -D COD_VERSION=COD2_1_2"

elif [ "$1" == "cod2_1_3" ]; then
	constants="-D COD2_VERSION=COD2_VERSION_1_3 -D COD_VERSION=COD2_1_3"

elif [ "$1" == "cod4_1_7" ]; then
	constants="-D COD_VERSION=COD4_1_7"

elif [ "$1" == "cod4_1_7_l" ]; then
	constants="-D COD_VERSION=COD4_1_7_L"

elif [ "$1" == "" ]; then
	echo "##### Please specify a command line option #####"
	exit 0

else
	echo "##### Unrecognized command line option $1 #####"
	exit 0
fi

mkdir -p bin
mkdir -p objects_$1
echo "##### COMPILE $1 LIBCOD.CPP #####"
$cc $options $constants -o objects_$1/libcod.opp -c libcod.cpp
echo "##### COMPILE $1 GSC.CPP #####"
$cc $options $constants -o objects_$1/gsc.opp -c gsc.cpp
echo "##### COMPILE $1 GSC_PLAYER.CPP #####"
$cc $options $constants -o objects_$1/gsc_player.opp -c gsc_player.cpp
echo "##### COMPILE $1 GSC_UTILS.CPP #####"
$cc $options $constants -o objects_$1/gsc_utils.opp -c gsc_utils.cpp

echo "##### LINK lib$1.so #####"
objects=""
if [ -e objects_base ]; then
	objects="$(ls objects_base/*.opp) "
else
	echo "##### ERROR: objects_base not found, you proably forgot to ./doit.sh base #####"
	exit 0
fi
objects+="$(ls objects_$1/*.opp)"
if [ -e objects_tcc ]; then
	objects+=" $(ls objects_tcc/*.opp)"
fi
if [ -e objects_car ]; then
	objects+=" $(ls objects_car/*.opp)"
fi
$cc -m32 -shared -L/lib32 $mysql_link -o bin/lib$1.so -ldl $objects $java_lib $mysql_config

if [ "$mysql_enable" == "false" ]; then
	sed -i "/#define COMPILE_MYSQL 0/c\#define COMPILE_MYSQL 1" config.hpp
	sed -i "/#define COMPILE_MYSQL_TESTS 0/c\#define COMPILE_MYSQL_TESTS 1" config.hpp
fi
