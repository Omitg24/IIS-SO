#define NUMERO_DE_ESTADOS 4

#define NUMERO_DE_TRANSICIONES 4

enum estados {LISTO, EJECUCION, BLOQUEADO, FINALIZADO};

enum eventos {DESPACHAR, BLOQUEAR, DESPERTAR, FINALIZAR};


int cambiarEstado(int estadoActual, int eventoOcurrido);

char * getNombreEstado(int);

char * getNombreTransicion(int);

