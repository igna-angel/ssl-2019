Compilado con los siguientes comandos
bison -d bisonMicro.y
flex flexMicro.l
gcc -o tp4exe bisonMicro.tab.c lex.yy.c

Instrucciones de uso:
En esta carpeta hay 2 archivos de prueba, llamados src_ok.mk y src_error.mk
Invocar al compilador de la siguiente forma
./tp4exe <archivo_micro>

Siendo
<archivo_micro> uno de los provistos, o cualquier otro con sintaxis micro para validar

