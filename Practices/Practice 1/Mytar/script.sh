#! /bin/bash

TEMPO=tmp
DOUT=out
FILE1=file1.txt
FILE2=file2.txt
FILE3=file3.dat
TAR_FILE=filetar.mtar

#Comprobar que mytar esta en el directorio actual
if [ -e ./mytar ]; then 
#Comprobar que mytar es ejecutable
	if [ -x ./mytar ]; then

#Comprobar si existe un directorio tmp dentro del directorio actual
		if [-d $TEMPO ]; then
			rm -rf $DIRE
			echo "$TEMPO directory deleted!"
		fi
		mkdir $TEMPO 
		cd $TEMPO
		
		if [ ! -e $FILE1 ]; then
			touch $FILE1
			echo "Hello World!" > $FILE1
			echo "File 1 created!"
		fi

		if [ ! -e $FILE2 ]; then
			touch $FILE2
			head -10 /etc/passwd > $FILE2
			echo "File 2 created!"
		fi

		if [ ! -e $FILE3 ]; then
			touch $FILE3
			head -c 1024 /dev/urandom > $FILE3
			echo "File 3 created!"
		fi

		./../mytar -c -f $TAR_FILE $FILE1 $FILE2 $FILE3


		if [ ! -d $DOUT ]; then
			mkdir $DOUT

			echo "$DOUT directory created!"
		fi

		cp $TAR_FILE $DOUT


		cd $DOUT
		./../../mytar -x -f $TAR_FILE


		diff ../$FILE1 $FILE1

		if [ $? = 0 ]; then
			echo "$FILE1 is identical"
			diff ../$FILE2 $FILE2
			
			if [ $? = 0 ]; then
				echo "$FILE2 is identical"
				diff ../$FILE3 $FILE3

				if [ $? = 0 ]; then
					echo "$FILE3 is identical"
					cd ../..
					echo "Correct!"
					exit 0
			    else
			        echo "Error: $FILE3 is different"
				    cd ../..
			        exit 1
                fi
        	else
        		echo "Error: $FILE2 is different"
        		cd ../..
        		exit 1
	        fi
else
	echo "Error: $FILE1 is different"
	cd ../..
	exit 1
fi
			
	else
		echo "ERROR: mytar file is not an executable"
		exit 1
	fi
else
	echo "ERROR: mytar file doesn't exist."
	exit 1
fi
