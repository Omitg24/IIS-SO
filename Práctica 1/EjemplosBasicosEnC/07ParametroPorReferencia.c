#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CADENA_POR_DEFECTO "Fernando"

#define POSICION_CADENA_EN_LINEA_COMANDOS 1

main (int argc, char *argv[]) {
  
  int longitud=strlen(CADENA_POR_DEFECTO);
  
  if (argc==2)
    longitud=strlen(argv[POSICION_CADENA_EN_LINEA_COMANDOS]);
  
  sustituyeEnteroPorSuCuadrado(&longitud);
  
  printf("La longitud de la cadena al cuadrado es %d\n",longitud);
  exit(1);
}

void sustituyeEnteroPorSuCuadrado(int *longi) {
  
  (*longi) *= (*longi);
}

// 1. Compilar con "gcc 07ParametroPorReferencia.c"
// 2. Ejecutar con "./a.out"
// 3. Implementa otra función parecida a sustituyeEnteroPorSuCuadrado pero pasando el argumento por valor; estudia la diferencia de comportamiento

