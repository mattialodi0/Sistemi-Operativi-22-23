#include ``/usr/include/umps3/umps/libumps.h''

//scelta pcb 
//rimozione pcb dalla testa e tornare il puntatore al pcb nel campo dei processi correnti
//caricare 5 millisecondi come time slice
//creare un Load Processor State sullo stato del processore memorizzato nel pcb del processo corrente
//cambio stato (?)
//se il process count è 0 richiama l'HALT BIOS
//se il process count è > 0 e il Soft-Block Count > 0 va in Wait State 
//controllo se process count > 0 e Soft-Block Count == 0 => deadlock. ???. Richiamare PANIC BIOS.
