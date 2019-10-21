%{
//Se incluyen los header correspondiente.
#include <string.h>
#include <stdlib.h>
#include "bisonMicro.tab.h"
%}


PALABRA_RESERVADA (inicio|fin|leer|escribir)
LETRA [a-zA-Z]
DIGITO  [0-9]
OPERADOR_ADITIVO    [+-]
ASIGNACION  :=

%%

[(),;]                          return yytext[0];
inicio                          return INICIO;
fin                             return FIN;
leer                            return LEER;
escribir                        return ESCRIBIR;
{LETRA}({LETRA}|{DIGITO})*      {
                                    //Almacenamos el nombre del identificador reconocido para luego poder acceder al mismo desde bison.
                                    //Si no se utiliza strdup() el puntero de yytext será copiado en yylval.identificador.
                                    yylval.identificador = strdup(yytext); 
                                    return IDENTIFICADOR; 
                                }
{DIGITO}{DIGITO}*               return CONSTANTE;
{OPERADOR_ADITIVO}              return OPERADOR_ADITIVO;
{ASIGNACION}                    return ASIGNACION;
[ \t\n]+                        /* ignora espacios y enter*/;

