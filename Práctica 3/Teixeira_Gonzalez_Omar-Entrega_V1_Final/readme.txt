Omar Teixeira González:

Ejercicios 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 y 16.

Ejercicio 3:
No se ejecuta 2 veces debido a la instrucción HALT, para ello se utiliza la instrucción TRAP 3.

Ejercicio 4:
Queda en ejecución el SystemIdleProcess, que ejecuta un bucle infinito que no hace nada, 
de esta forma el procesador no está parado, ya que, el numero de programas es mayor al soportado.

Ejercicio 6:
Debido a que este supera al máximo reservado en memoria principal.

Ejercicio 13:
	a. Porque cuando se produce un cambio de contexto es necesario guardar esos valores 
	para cuando se vuelva a reanudar el proceso.
	b. Sí, sería necesario guardar el resto de registros del procesador
	(ya que los datos de la pila ya están guardados), como el acumulador.
	c. Si, ya que, solo guarda los registros PC y PSW del procesador, en lugar de 
	todos los posibles (como el acumulador).
	d. Si, ha sido necesario modificar los métodos OparatingSyste_SaveContext() y 
	OperatingSystem_RestoreContext(), además de la estructura del PCB, añadiendo una
	copia del registro acumulador.

Ejercicio 14:
Si el proceso a terminar es de tipo usuario, el numero de procesos no terminados se decrementará 
en una unidad, y, cuando este número llegue a 0 si el proceso en ejecución es el Idle del sistema,
se provocará que termine dicho proceso y el sistema avisará de que se apagará, para ello, se avisa al 
sipID que debe finalizar.
En caso de que el numero de procesos no terminados no sea 0, se seleccionara el siguiente proceso a 
ejecutar y se le asignará al procesador.

Ejercicio 16:
El sistema se apaga debido a que dichas instrucciones se usan en el caso de que se ejecute una 
interrupción, y, como en los programas probados (como ejemplo) no se provocaba ninguna interrupción,
el sistema debia apagarse.
	b. Cuando se llama al trap, ya que es cuando se activa el bit de protegido.

:-)
