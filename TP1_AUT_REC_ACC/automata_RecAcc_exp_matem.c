//funcion en c: automata reconocedor y accionador
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int columna(char s){
	if(s=='+' || s=='-') return 0;
	if(s=='0') return 1;
	if(s>='1' && s<='9') return 2;
	if(s=='*') return 3;
	return 4;
}
int automata (char s[]){
	int MT [5][5]= {{2,1,3,4,4},{0,4,4,0,4},{4,4,3,4,4},{0,3,3,0,4},{4,4,4,4,4}};
	int i=0;
	int e=0;
	while (s[i] != '\0'){

		e=MT[e][columna(s[i])];
		i++;
	}
	if(e==1 || e == 3) return 1;
	return 0;

}
void reconocedorAccionador(char s[]){
	int i, j, numActual, sumaTotal, producto;
	char signoActual, signoAnterior;
	int numerosAOperar[400];
	if(automata(s)==1){ // Se reconoce OK la cadena y se puede operar
        producto = 1;
        numActual = 0;
        sumaTotal = 0;
        i = 0;
        j = 0;
        signoActual = '+';
        signoAnterior = '+';


        while (s[i] != '\0') {
            if (s[i] == '+' || s[i] == '-' || s[i] == '*') {
                signoAnterior = signoActual;
                signoActual = s[i];

                if (signoActual == '*') {
                    if (signoAnterior == '-') {
                        numActual = -numActual;
                    }
                    producto *= numActual;
                    numActual = 0;
                }
            }

            else { // Es un número
                numActual = numActual*10 +(s[i]-'0');

                if (s[i+1] == '\0' || s[i+1] == '+' || s[i+1] == '-') { // Ultimo número o se viene suma/resta
                    if (signoActual == '*' || signoAnterior == '*') {
                        if (signoActual == '-') {
                            numActual = -numActual;
                        }

                        numerosAOperar[j] = producto * numActual;
                    }

                    else if (signoActual == '-') {
                        numActual = -numActual;
                        numerosAOperar[j] = numActual;
                    }

                    else if (signoActual == '+') {
                        numerosAOperar[j] = numActual;
                    }

                    producto = 1;
                    numActual = 0;
                    j++;
                }
            }

            i++;
        }

        // Ahora solo queda sumar los elementos del vector de números a operar
        for (int n = 0; n < j; n++) {
            sumaTotal += numerosAOperar[n];
        }

        printf("\r\nmuestra por pantalla: %d",sumaTotal);
	}

	else printf("no se puede leer la cadena");
	return;
}
int main (){
	char s[400];
    	printf("Dame la cadena a verificar: ");
    	fgets(s, sizeof s, stdin);
	s[strcspn(s, "\n")] = '\0';
	int resultado=automata(s);
	printf("el resultado: %d \r\n",resultado);
	reconocedorAccionador(s);
	return 0;
}
