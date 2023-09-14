#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANIOMAXIMO 20

struct persona {
  char nombre[TAMANIOMAXIMO];
  int edad;
  float ingresos;
}

main (int argc, char *argv[]) {
  
  struct persona profesor={"Fernando", 23, 777.11};
  
  struct persona alumno;
  
  strcpy(alumno.nombre,"Juan");
  alumno.edad=19;
  alumno.ingresos=11.777;
  
  printf("Los ingresos conjuntos de alumnno y profesor son %f\n",alumno.ingresos+profesor.ingresos);
  exit(1);
}

// 1. Compilar con "gcc 08Estructura.c"
// 2. Ejecutar con "./a.out"
// 3. Modifica el programa para que el nombre, edad e ingresos del profesor se obtengan de línea de comandos

