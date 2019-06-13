#!/bin/bash
DTEMP="./temp"
DSRC="./src" 
DMOUNT="./mount-point" 
FILE1="myFS.h"
FILE2="fuseLib.c"
FILE3="MyFileSystem.c"
#a) Copie dos ficheros de texto que ocupen más de un bloque (por ejemplo fuseLib.c y myFS.h) a nuestro SF y a un directorio temporal, por ejemplo ./temp

#Comprobamos si hay un Directorio Temporal para regenerarlo
if test -e $DTEMP; then
	rm -r $DTEMP
fi

mkdir $DTEMP

cp $DSRC/$FILE1 $DTEMP/$FILE1
cp $DSRC/$FILE1 $DMOUNT/$FILE1

cp $DSRC/$FILE2 $DTEMP/$FILE2	
cp $DSRC/$FILE2 $DMOUNT/$FILE2


#b) Audite el disco y haga un diff entre los ficheros originales y los copiados en el SF.
./my-fsck-static virtual-disk


diff $DMOUNT/$FILE1 $DTEMP/$FILE1

if [ $? = 0 ]; then 
	echo "The $FILE1 is the same for both directories ($DMOUNT y $DTEMP )"

    diff $DMOUNT/$FILE2 $DTEMP/$FILE2
	if [ $? = 0 ]; then
		echo "The $FILE2 is the same for both directories ($DMOUNT y $DTEMP )"
	else
		echo "The $FILE2 is not the same for both directories ($DMOUNT y $DTEMP )"
fi
else 
	echo "The $FILE1 is not the same for both directories ($DMOUNT y $DTEMP )"
fi


#Trunque el primer fichero (man truncate) en copiasTemporales y en nuestro SF de manera que ocupe ocupe un bloque de datos menos.
truncate --size=-4096 $DTEMP/$FILE1
truncate --size=-4096 $DMOUNT/$FILE1

echo "File 1 truncated"



#c) Audite el disco y haga un diff entre el fichero original y el truncado.
./my-fsck-static virtual-disk


diff $DMOUNT/$FILE1 $DSRC/$FILE1
if [ $? = 0 ]; then 
	echo "The $FILE1 is the same for both directories ($DMOUNT y $DSRC)"

else 
	echo "The $FILE1 is not the same for both directories"
fi

#d) Copie un tercer fichero de texto a nuestro SF.
cp $DSRC/$FILE3 $DMOUNT/$FILE3

#e) Audite el disco y haga un diff entre el fichero original y el copiado en el SF
diff $DSRC/$FILE3 $DMOUNT/$FILE3

if [ $? = 0 ]; then 
	echo "The $FILE3 is the same for both directories ($DSRC y $DMOUNT )"
else 
	echo "The $FILE3 is not the same for both directories ($DSRC y $DMOUNT )"
fi


#f ) Trunque el segundo fichero en copiasTemporales y en nuestro SF haciendo que ocupe algún bloque de datos más.
truncate --size=+4096 $DTEMP/$FILE2
truncate --size=+4096 $DMOUNT/$FILE2

echo "File 2 truncated"

#g) Audite el disco y haga un diff entre el fichero original y el truncado.
./my-fsck-static virtual-disk


diff $DIRECTORY_S/$FILE2 $DIRECTORY_M/$FILE2

if [ $? = 0 ]; then 
	echo "The $FILE2 is the same for both directories ($DSRC y $DMOUNT )"
else 
	echo "The $FILE2 is not the same for both directories ($DSRC y $DMOUNT )"
fi


diff $DSRC/$FILE2 $DTEMP/$FILE2

if [ $? = 0 ]; then 
	echo "The $FILE2 is the same for both directories ($DSRC y $DTEMP )"
else 
	echo "The $FILE2 is not the same for both directories ($DSRC y $DTEMP )"
fi
