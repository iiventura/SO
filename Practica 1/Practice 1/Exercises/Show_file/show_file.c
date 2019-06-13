#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char* argv[]) {
	FILE* file=NULL;

	if (argc!=3) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	/* Open file */
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);

	long lSize;
	char * buffer;
	size_t result;
	// Convertimos el número de bloques a entero
	size_t block = atoi(argv[2]);

	// Asignamor un pntero a la dir del fichero de longitud
	fseek (file , 0, block);
	// Cogemos el tamaño del fichero
	lSize = block;
	// Colocamos un puntero al inicio del fichero
	rewind (file);

	// Reservamos memoria para contener el fichero
	buffer = (char*) malloc (sizeof(char)*lSize);
	if (buffer == NULL) {fputs ("ERROR: Al reservar memoria",stderr); exit (2);}

	// Copiamos el fichero en el buffer
	result = fread (buffer,1,lSize,file);
	if (result != lSize) {fputs ("ERROR: Leemos más de lo que contiene el fichero",stderr); exit (3);}

	// Mostramos el buffer
	fwrite(buffer, lSize, 1, stdout);

	// terminate
	fclose (file);
	free (buffer);

	/*
	while ((c = getc(file)) != EOF) {
		/* Print byte to stdout
		ret=putc((unsigned char) c, stdout);

		if (ret==EOF){
			fclose(file);
			err(3,"putc() failed!!");
		}
	}
	*/

	return 0;
}
