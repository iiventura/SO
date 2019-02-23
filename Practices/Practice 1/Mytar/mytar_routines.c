#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"

extern char *use;

/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
int
copynFile(FILE * origin, FILE * destination, int nBytes)
{
	// Transfiere nBytes del fichero origen al fichero destino
	
	int byte = 0;
	int count = 0;
	int line = 0;
	while ((line != EOF) && (count < nBytes)) {
		line = getc(origin);
		if (line != EOF) {
			byte = putc((unsigned char) line, destination);

			if (byte == EOF) {
				perror("putc() failed!!");
				return -1;
			}
			count++;
		}
	}
	return count;
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char*
loadstr(FILE * file)
{

	// Lee una cadena de caracteres del fichero cuyo descriptor se pasa como parámetro
	// Usar esta función en la implementación de readHeader()
	// La función reserva memoria para la cadena leída. La dirección de
	// memoria donde comienza la cadena se devuelve como valor de retorno.

	int count = 0;
	int line;
	while ((line = getc(file)) != '\0')
		count++;

	char *name = NULL;
	if (count > 0) {
		name = (char *)malloc(sizeof(char) * (count + 1));

		fseek(file, -(count+1), SEEK_CUR);

		int i = 0;
		for (; i < count + 1; i++)
			name[i] = getc(file);
	}
	return name;
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
stHeaderEntry*
readHeader(FILE * tarFile, int *nFiles)
{
	// La memoria para el array ha de reservarse con malloc() en el interior de esa función
	// Devuelve en nFiles (entero por referencia) el número de ficheros contenidos en el mtar

	int numberFiles = 0;
	if(fread(&numberFiles, sizeof(int), 1, tarFile) == 0)
		perror("Error reading the number of files\n");

	stHeaderEntry * fileList = malloc(sizeof(stHeaderEntry) * numberFiles);

	unsigned int size;
	char * name = loadstr(tarFile);
	int i = 0;
	if (name != NULL) {
		fileList[i].name = name;
		if(fread(&size,sizeof(unsigned int), 1, tarFile) > 0)
			fileList[i].size = size;
		i++;
	}

	while ((i < numberFiles) && (name != NULL)) {
		name = loadstr(tarFile);
		if (name != NULL) {
			fileList[i].name = name;
			if(fread(&size,sizeof(unsigned int), 1, tarFile) > 0)
				fileList[i].size = size;
			i++;
		}
	}

	(*nFiles) = numberFiles;

	return fileList;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int
createTar(int nFiles, char *fileNames[], char tarName[])
{
	// Reservamos memoria (con malloc()) para un array de stHeaderEntry
	stHeaderEntry* fileList = malloc(sizeof(stHeaderEntry) * nFiles);

	// Abrimos el fichero mtar para escritura (fichero destino)
	FILE * mtar;
	if ((mtar = fopen(tarName, "w")) == NULL) return EXIT_FAILURE;

	// Inicializar campo name de cada estructura stHeaderEntry
	int i = 0;
	int lon = 0;
	int bytesCount = 0;
	// Exige reservar memoria para alojar la cadena asociada a cada
	// nombre de fichero (No olvidar reservar espacio para el '\0')
	for (; i < nFiles; i++) {
		// lon = longitud del nombre del fichero
		lon = strlen(fileNames[i]) + 1;

		// Reserva espacio para lon
		fileList[i].name = malloc(lon);

		// Escribimos el nombre en la lista
		strcpy(fileList[i].name, fileNames[i]);

		// bytesCount = cantidad de bytes que ocupan todos los nombres de los ficheros
		bytesCount += lon;
	}

	// Nos posicionamos en el byte del fichero donde comienza la región de datos
	int offSet = sizeof(int) + (nFiles * sizeof(unsigned int)) + bytesCount;

	// Asignamos el puntero al offSet del fichero mtar
	if (fseek(mtar, offSet, SEEK_SET) == -1) return EXIT_FAILURE;

	// Por cada fichero (inputFile) que haya que copiar en el mtar:
	FILE * inputFile;
	i = 0;
	for (; i < nFiles; i++) {
		// 1 Abrimos inputFile
		if ((inputFile = fopen(fileNames[i], "r")) == NULL) {
			fclose(mtar);
			return EXIT_FAILURE;
		}
		// 2 copynFile(inputFile, tarFile, INT_MAX)
		int bytes = copynFile(inputFile, mtar, INT_MAX);

		// 3 Cerramos inputFile
		fclose(inputFile);

		// 4 Rellenamos el elemento correspondiente del array de estructuras
		// con el tamaño del fichero que acabamos de volcar a disco
		fileList[i].size = bytes;
	}


	// Escribir el número de bytes que ocupa el fichero
	// Liberamos memoria y cerramos el fichero mtar

	// Nos posicionamos para escribir en el byte 0 del fichero tar para:
	if (fseek(mtar, 0, SEEK_SET) == -1) return EXIT_FAILURE;

	// 1 escribir número de ficheros en el fichero (4 bytes)
	if (fwrite(&nFiles, sizeof(int), 1, mtar) !=-1) {
		// 2 Para cada estructura stHeaderEntry:
		i = 0;
		for (; i < nFiles; i++) {
			// Escribir la ruta del fichero (con '\0' al final)
			if((fwrite(fileList[i].name, strlen(fileList[i].name) + 1, 1, mtar) != -1)
					&&(fwrite(&fileList[i].size, sizeof(unsigned int), 1, mtar) != -1))
				free(fileList[i].name);

			else return EXIT_FAILURE;
		}
		printf("Mtar file created successfuly\n");
	}
	free(fileList);
	fclose(mtar);

	return EXIT_FAILURE;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int
extractTar(char tarName[])
{
	FILE * tar;
	int nFiles;


	if((tar=fopen(tarName, "r")) != NULL){
		stHeaderEntry* fileList = readHeader(tar, &nFiles);

		if(fileList != NULL){
			int i=0;
			for(; i < nFiles; i++){
				char *name = fileList[i].name;
				FILE * outFile;
				if((outFile = fopen(name, "w")) != NULL){
					int bytes= copynFile(tar, outFile, fileList[i].size);

					if(bytes == fileList[i].size){
						printf("[%d] Creating file %s, size %d Bytes... OK\n",
								i,name, fileList[i].size);
						fclose(outFile);
					}else
						perror("Error writing in a file\n");


				}else
					perror("Error trying to open a file\n");


				free(name);
			}
			free(fileList);
		}else
			perror("Error reading the tarball's header\n");

		fclose(tar);
	}else
		perror("Error trying to open the tar file\n");
	return EXIT_FAILURE;

}
