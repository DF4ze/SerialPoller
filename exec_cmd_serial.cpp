/**************************************************



**************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "rs232.h"

void reset_string( unsigned char * String, int iLength );

int main(){
  // Pr la connexion au Port Serie
  int i, n,
      cport_nr=24,        // 22 ==> /dev/ttyAMA0 (fils) 24 ==> /dev/ttyACM0 (USB)
      bdrate=115200;       // 9600 baud 
  // Pr l'arrivée du Port Serie
  const int MAX_SIZE = 4096;
  unsigned char buf[MAX_SIZE];
  unsigned char bufTemp[MAX_SIZE];
  // char buf[MAX_SIZE];
  // char bufTemp[MAX_SIZE];
  bool bStrCompleted = false;

  
  
  // Ouverture du port
  if(RS232_OpenComport(cport_nr, bdrate))  {
    printf("Impossible d'ouvrir le port serie\n");

    return(0);
  }

  
  
  // Boucle de polling
  while(1){
	// Attente et récupération du buffer
    n = RS232_PollComport(cport_nr, bufTemp, MAX_SIZE);

	// a-t-on receptionné des données
	if( n != 0 ){
		// y a-t-il une retour a la ligne? ///// !!!! \\\\\ retour chariot se fait sous la forme : \r\n et non \n simplement.
		for(i=0; i < n; i++){
			if( bufTemp[i] == '\r' /* '\n' */ ){
				// int iTaille;
				// printf("Recu %i Octets, taille %i : %s\n", n, strlen((char *)bufTemp), (char *)bufTemp);
				
				// SI oui alors on a atteind la fin de la commande
				bStrCompleted = true;
				// On écrase le retour chariot 
				bufTemp[i] = 0;
				// bufTemp[i-1] = 0;
				
				// on arrete la boucle
				i=n;
				
				// Petit bug ... un caractère s'incruste au tout début....?!?!!
				// iTaille = strlen( (char *)bufTemp );
				// for( int j=0; j < iTaille; j++ ){
					// bufTemp[j] = bufTemp[j+1];
				// }
				// printf("BufTemp apres suppr 1er char (%i): %s\n", strlen(bufTemp), bufTemp);
				  // for(int j=0; j < iTaille; j++)
				  // {
					// if(bufTemp[j] < 32)  /* replace unreadable control-codes by dots */
					// {
					  // bufTemp[j] = '.';
					// }
				  // }
			}
		} 
		
		strcat( (char *)buf, (char *)bufTemp );
		// printf("Buf cat (%i): %s\n", strlen((char *)buf), (char *)buf);
	}

	// Si la chaine est complete alors on la traite
	if( bStrCompleted ){ 
//		buf[n] = 0;   // Toujours mettre un 0 de fin de chaine.

		

		// Execution de la commande s'il ne s'agit pas d'une info de debug
		if( buf[0] != '#' ){
			// Pr la commande et son resultat.
			FILE * pCommand;
			unsigned char sResult[MAX_SIZE] = {0};	
			printf("Buff avant popen (%i) : %s\n", strlen((char *)buf), (char *)buf);
			
			pCommand = popen( (char *)buf, "r");
		
			while (!feof(pCommand)) {
			  fread(sResult, 1, MAX_SIZE, pCommand); 
			}
			
			
			// On envoi le résultat sur le port serie avec un indicateur de fin de chaine '*'
			// strcat( (char *)sResult, "\n");
				// Pr debug, on affiche
				printf("Result : %s", sResult);
			RS232_SendBuf( cport_nr, sResult, MAX_SIZE );
			
			fclose(pCommand);
		}else
			printf("##Com : %s\n", buf);

		
		// remise a zero 
		reset_string( buf, MAX_SIZE );
		reset_string( bufTemp, MAX_SIZE );
		bStrCompleted = false;
	}
	
    usleep(100000);  /* sleep for 100 milliSeconds */

  }

  return(0);
}

void reset_string( unsigned char * String, int iLength ){
	for( int i=0; i < iLength; i++ ){
		String[i] = 0;
	}
}