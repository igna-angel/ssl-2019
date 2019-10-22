
/* Programa que utiliza una Maquina de Turing*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#define NUMESTADOS 10
#define NUMCOLS 7
#define ACEPTADA 1
#define RECHAZADA 0
#define D 1
#define I 2
int columna(int c);
int verifica(char *s);
int mturing(char *cadena);
/**************************************************************/
/* Elemento de la Matriz de transiciones de la maq. de Turing */
typedef struct{
    int marca;
    int moverA;
    int estadoSiguiente;
    }elementoMatriz;
/*******************************************************************/
/* Funcion Principal que lee una cadena de la linea de comandos    y decide si pertenece al lenguaje, utilizando la maq. de Turing */
int main(int argc, char *argv[])
{
     if ( argc == 1 )
    {
    printf("Debe ingresar una Cadena en Linea de Comandos\n");
    return EXIT_FAILURE;
    }
    if ( argc != 2 )
    {
        printf("Cantidad de Argumentos Incorrecta\n");
        return EXIT_FAILURE;
        }
        if ( !verifica(argv[1]) )
        {
            printf("Los Caracteres de la Cadena No pertenecen al Alfabeto\n");
            return EXIT_FAILURE;
        }
        if ( mturing(argv[1]) )
            printf("La Cadena ingresada pertenece al Lenguaje\n");
        else printf("La Cadena ingresada No pertenece al Lenguaje\n");
            return 0;
    }
/********************************************/
/* Funcion que implementa la maq. de Turing */
int mturing(char *cadena)
{
    static elementoMatriz tabla[NUMESTADOS][NUMCOLS] = {
        {
            { 'X', D, 1}, {0, 0, 9},{0, 0, 9}, {0, 0, 9}, {'Y', D, 6},{0, 0, 9 }, { 0, 0, 9 }
        },
        {
            { 'a', D, 1 },{ 'Y', D, 2 },{ 0, 0, 9 }, { 0, 0, 9 },{'Y', D, 4 },{ 0, 0, 9}, { 0, 0, 9 }
        },
        {
            { 0, 0, 9 }, { 'b', D, 2 },{ 'Z', I, 3 },{ 0, 0, 9 },{ 0, 0, 9 }, {'Z', D, 5 }, {0, 0, 9 }
        },
        {
            { 'a', I, 3}, { 'b', I, 3 },{ 0, 0, 9 }, {'X', D, 0},{'Y', I, 3 },{ 'Z', I, 3 },{0, 0, 9 }
        },
        {
            { 0, 0, 9 }, { 'Y', D, 2 }, { 0, 0, 9 }, { 0, 0, 9 }, { 'Y', D, 4 }, { 0, 0, 9 },{0, 0, 9 }
        },
        {
            { 0, 0, 9 }, { 0, 0, 9 }, { 'Z', I, 3 }, { 0, 0, 9 }, { 0, 0, 9 }, { 'Z', D, 5 },{0, 0, 9 }
        },
        {
            { 0, 0, 9 }, { 0, 0, 9 }, { 0, 0, 9 }, { 0, 0, 9 }, { 'Y', D, 6 }, { 'Z', D, 7 },{0, 0, 9 }
        },
        {
            { 0, 0, 9 }, { 0, 0, 9 }, { 0, 0, 9 }, { 0, 0, 9 }, { 0, 0, 9 }, { 'Z', D, 7 }, { 0, I, 8 }
        },
        {
            { 0, 0, 9 }, { 0, 0, 9 }, { 0, 0, 9 }, { 0, 0, 9 }, { 0, 0, 9 }, { 0, 0, 9 }, { 0, 0, 9 }
        },
        {
            { 0, 0, 9 }, { 0, 0, 9 }, { 0, 0, 9 }, { 0, 0, 9 }, { 0, 0, 9 }, { 0, 0, 9 }, { 0, 0, 9 }
        }
        };
        int estadoActual = 0;
        int moverA = D;
        char *punteroCar = cadena;
        int caracter = *punteroCar;
        int paso=1;
        printf("Inicio maquina: %s\n",cadena);
        printf("Estado Actual: %d",estadoActual);
        printf("\tCaracter: %c",*punteroCar);
        printf("\tMover a Derecha\n");
        printf("- - - - - - -\n");
        while ( caracter )
            {
                printf("Paso: %d\n",paso);
                *punteroCar = tabla[estadoActual][columna(caracter)].marca;
                moverA = tabla[estadoActual][columna(caracter)].moverA;
                estadoActual = tabla[estadoActual][columna(caracter)].estadoSiguiente;
                printf("Estado Actual: %d",estadoActual);
                printf("\tCaracter: %c",*punteroCar);


                if ( moverA == D ){
                    printf("\tMover a Derecha\n");
                    ++punteroCar;
                }

                if ( moverA == I ){
                        printf("\tMover a Izquierda \n");
                    --punteroCar;
                }
                caracter = *punteroCar;
                paso++;
                printf("- - - - - - -\n");
            }

        estadoActual = tabla[estadoActual][columna(caracter)].estadoSiguiente;
        printf("Estado Final: %d\n",estadoActual);
        if ( estadoActual == 8 )
            return ACEPTADA;
        else return RECHAZADA;
}
/***********************************************************************/
/* Funcion que retorna la columna de la matriz que corresponde al caracter leido */
int columna(int c)
{
    switch ( c )
    {
        case 'a' : return 0;
        case 'b' : return 1;
        case 'c' : return 2;
        case 'X' : return 3;
        case 'Y' : return 4;
        case 'Z' : return 5;
        case '\0' : return 6;
    }
}/***********************************************************************/
/* Funcion que verifica si los caracteres de la cadena pertenecen al alfabeto */
int verifica(char *s)
{
    size_t n = strlen(s);
    if ( s[n -1] == '\n' )
        s[n -1] = '\0';
    for ( ; *s; ++s ){
        if ( !(*s == 'a' || *s == 'b'|| *s == 'c' ) )
         return 0;
    }
    return 1;
}
