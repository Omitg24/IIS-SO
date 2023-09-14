#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define CADENA_POR_DEFECTO "Fernando Alvarez Garcia"

#define POSICION_CADENA_EN_LINEA_COMANDOS 1

main (int argc, char *argv[]) {
  
  char cadena[]=CADENA_POR_DEFECTO;
  
  if (argc==2)
      strcpy(cadena, argv[POSICION_CADENA_EN_LINEA_COMANDOS]);
  
  pasarAMayusculas(cadena);
  printf("La cadena de caracteres es ahora %s\n",cadena);
  
  exit(1);
}

void pasarAMayusculas(char cadena[]) {
  
  int i;
  int longitud=strlen(cadena);
  
  for (i=0; i<longitud;i++)
    cadena[i]=toupper(cadena[i]);
}


// 1. Compilar con "gcc 05PasarAMayusculas.c"
// 2. Ejecutar con "./a.out"
// 3. Obtener la ayuda sobre strcpy, strlen, toupper: "man strcpy", "man strlen", "man toupper"
// 4. Modificar el programa para que sólo pase a mayúsculas las 10 primeras letras del alfabeto