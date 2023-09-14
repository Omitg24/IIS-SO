#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CARACTER_POR_DEFECTO 'a'
#define CADENA_POR_DEFECTO "Fernando Alvarez Garcia"

#define POSICION_CADENA_EN_LINEA_COMANDOS 1
#define POSICION_CARACTER_EN_LINEA_COMANDOS 2

main (int argc, char *argv[]) {
  
  if (argc==1)
    buscarYContar(CARACTER_POR_DEFECTO, CADENA_POR_DEFECTO);
  else
    if (argc==2)
      buscarYContar(CARACTER_POR_DEFECTO, argv[POSICION_CADENA_EN_LINEA_COMANDOS]);
    else
      buscarYContar(argv[POSICION_CARACTER_EN_LINEA_COMANDOS][0], argv[POSICION_CADENA_EN_LINEA_COMANDOS]);
  exit(1);
}

void buscarYContar(char caracter, char *cadena) {
  
  int i;
  int longitud=strlen(cadena);
  
  printf("Buscando en la cadena %s\n",cadena);
  for (i=0;i<longitud;i++)
    if (cadena[i]==caracter)
      printf("La posición %d de la cadena contiene el caracter [%c]\n",i,caracter);
}
  
// 1. Compilar con "gcc 06BuscarCaracter.c"
// 2. Ejecutar con "./a.out"
// 3. Modificar la función buscarYContar para que cuente y devuelva el número de caracteres encontrados en la cadena


  