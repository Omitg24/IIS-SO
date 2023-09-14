#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "10Estados.h"

main (int argc, char *argv[]) {
  
  int estado;
  int transicion;
  int i;
  
  if (argc<2) {
    printf("Sintaxis: %s numeroDeEstado numeroDeTransicion\n",argv[0]);
    for (i=0;i < NUMERO_DE_ESTADOS;i++)
      printf("%s (%d),",getNombreEstado(i),i);
    printf("\n");
    for (i=0;i < NUMERO_DE_TRANSICIONES;i++)
      printf("%s (%d),",getNombreTransicion(i),i);
    printf("\n");
  }    
  else {
    estado=atoi(argv[1]);
    transicion=atoi(argv[2]);
    printf("La transición %s cambia al proceso del estado %s al estado %s\n", getNombreTransicion(transicion),getNombreEstado(estado),getNombreEstado(cambiarEstado(estado,transicion)));
  }
  exit(1);
}


// 1. Compilar con "gcc 10Estados.c 10PrincipalEstados.c"
// 2. Ejecutar con "./a.out"
// 3. Añade una transición a la función cambiarEstado que siempre ponga a un proceso en el estado FINALIZADO si se realiza la transición FINALIZAR
// 4. Modificar el programa para que detecte transiciones que no son tales, es decir, que no cambian el estado del proceso. Que muestre un mensaje especial en dicho caso.

