#include <stdio.h>
#include <stdlib.h>

main () {

  int edad=23;
  float dineroEnElBanco= 300.17;
  char letraDelNIF='T';
   
  printf("Hola %s. A tus %d años, ¿qué vas a hacer con %f euros?\n", "Fernando",edad,dineroEnElBanco/2);
  printf("Mejor renueva tu DNI, ese que termina en %c\n",letraDelNIF);
  exit(1);
  
}


// 1. Compilar con "gcc 02HolaMundo.c"
// 2. Ejecutar con "./a.out"
// 3. Modificar el programa para que muestre la mitad del "dineroEnElBanco"
