#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define NUMEROESTADOS 15
#define NUMEROCOLUMNAS 13
#define TAMLEX 32
#define TAMTS 1000


typedef enum /*Tokens*/
	{
	    INICIO, FIN, LEER, ESCRIBIR, ID, CONSTANTE, PARENIZQUIERDO, PARENDERECHO, PUNTOYCOMA, COMA, ASIGNACION, SUMA, RESTA, FDT, ERRORLEXICO
	} TOKEN;

typedef struct /*Registro Tabla de Simbolos*/
	{
	    char identificadorDeLexema[TAMLEX+1];
	    TOKEN t;
	} RegTS;


typedef struct /*Registro Expresion*/
	{
	    TOKEN clase;
	    char nombre[TAMLEX+1];
	    int valor;
	} REG_EXPRESION;

typedef struct /*Registro Operacion*/
	{
	    TOKEN clase;
	    char operador;
	} REG_OPERACION;

/*Prototipos funciones Scanner*/
int columna(int c);
int estadoFinal(int e);
TOKEN scanner();

/*Prototipos PAS*/
void Objetivo(void);
void Programa(void);
void ListaSentencias(void);
void Sentencia(void);
void ListaIdentificadores(void);
void ListaExpresiones(void);
void Expresion(REG_EXPRESION * resultado);
void Primaria(REG_EXPRESION * resultado);
void OperadorAditivo(REG_OPERACION * op);
void Identificador(REG_EXPRESION * resultado);

/*Prototipos funciones auxiliares*/
void Match(TOKEN t);
TOKEN ProximoToken();
void ErrorLexico();
void ErrorSintactico();
void Generar(char * co, char * a, char * b, char * c);
char * Extraer(REG_EXPRESION * reg);
int Buscar(char * id, TOKEN * t);
void Colocar(char * id);
void Chequear(char * s);
void Comenzar(void);
void Terminar(void);
void Asignar(REG_EXPRESION izq, REG_EXPRESION der);

/*Prototipos rutinas semanticas*/
REG_EXPRESION ProcesarCte(void);
REG_EXPRESION ProcesarId(void);
REG_OPERACION ProcesarOp(void);
void Leer(REG_EXPRESION in);
void Escribir(REG_EXPRESION out);
REG_EXPRESION GenInfijo(REG_EXPRESION e1, REG_OPERACION op, REG_EXPRESION e2);

FILE * in;
FILE * out;
RegTS TS[];
char buffer[];
TOKEN tokenActual;

RegTS TS[TAMTS] = { {"inicio", INICIO}, {"fin", FIN}, {"leer", LEER}, {"escribir", ESCRIBIR}, {"$", 0} }; /*tabla de simbolos inicial con solo las palabras reservadas y el centinela al final*/

char buffer[TAMLEX+1]; /*buffer para guardar identificadores y constantes*/
int flagToken = 0;

int main(int argc, char * argv[]){
	int l;
	printf("COMPILADOR DE MICRO\n");
    /*Validaciones*/
	if ( argc == 1 )
	    {
	        printf("Debe ingresar el nombre del archivo entrada en lenguaje Micro y el nombre del archivo de salida\n");
	        return -1;
	    }
	if ( argc !=3 )
	    {
	        printf("Numero incorrecto de argumentos.\n");
	        return -2;
	    }
	l = strlen(argv[1]);
	if ( argv[1][l-1] != 'm' || argv[1][l-2] != '.' )
	    {
	        printf("El archivo de entrada debe tener extension .m \n");
	        return -3;
	    }
	if ( (in = fopen(argv[1], "r") ) == NULL )
	    {
	        printf("No se pudo abrir archivo de entrada\n");
	        return -4;
	    }
	if ( (out = fopen(argv[2], "w") ) == NULL)
	    {
	        printf("No se pudo abrir archivo de salida\n");
	        fclose(in);
	        return -5;
	    }

    /*Inicio PAS */
    Objetivo();

    /*Cierre*/
    fclose(in);
    fclose(out);
	return 0;
}
/*FUNCIONES AUXILIARES*/

void Match(TOKEN t)
{
    if ( !(t == ProximoToken()) ) ErrorSintactico(); /*lanza error sintactico si el proximo token no coincide*/
    flagToken = 0; /*limpia el flag una vez que hizo el match */
}

TOKEN ProximoToken()
{
    if ( !flagToken ) /*solo obtiene un nuevo token si se matcheo el anterior*/
    {
        tokenActual = scanner();
        if ( tokenActual == ERRORLEXICO ) ErrorLexico(); /*lanza error lexico si el scanner devuelve ese token*/
        flagToken = 1; /*setea el flag para indicar que un nuevo token ha de ser matcheado*/
        if ( tokenActual == ID ) Buscar(buffer, &tokenActual); /*si el token es alfanumerico llama a Buscar para buscarlo en la tabla de simbolos*/
    }
    return tokenActual;
}

void ErrorLexico()
{
    printf("Error Lexico\n");
    fprintf(out, "Error Lexico\n");

}

void ErrorSintactico()
{
    printf("Error Sintactico\n");
    fprintf(out, "Error Sintactico\n");
}

void Generar(char * accion, char * a, char * b, char * c)
{
    /* Produce la salida de la instruccion para la MV por pantalla y por archivo de salida */
    printf("%s %s%c%s%c%s\n", accion, a, ',', b, ',', c);
    fprintf(out,"%s %s%c%s%c%s\n", accion, a, ',', b, ',', c );
}

char * Extraer(REG_EXPRESION * reg)
{
    /* Retorna la cadena del registro semantico */
    return reg->nombre;
}

int Buscar(char * id, TOKEN * t)
{
    int i = 0;
    while ( strcmp("$", TS[i].identificadorDeLexema) )
    {
        if ( !strcmp(id, TS[i].identificadorDeLexema) )
        {
            *t = TS[i].t;
            return 1;
        }
        i++;
    }
    return 0;
}

void Colocar(char * id)
{
    /* Agrega un identificador a la TS */
    int i = 4;
    while ( strcmp("$", TS[i].identificadorDeLexema) ) i++; /*recorre la tabla hasta el centinela*/
    {
        if ( i < (TAMTS -1) ) /*si hay espacio ingresa el nuevo id*/
        {
            strcpy(TS[i].identificadorDeLexema, id );
            TS[i].t = ID;
            strcpy(TS[++i].identificadorDeLexema, "$" );
        }
    }
}

void Chequear(char * s)
{
    TOKEN t;
    if ( !Buscar(s, &t) )
    {
        Colocar(s);
        Generar("Declara", s, "Entera", "");
    }
}

void Comenzar(void)
{
    /* Inicializaciones Semanticas */

}

void Terminar(void)
{
    /* Genera la instruccion para terminar la ejecucion del programa */
}

void Asignar(REG_EXPRESION izq, REG_EXPRESION der)
{
    /* Genera la instruccion para la asignacion */
    Generar("Almacena", Extraer(&der), izq.nombre, "");
}

char buffer[];
TOKEN tokenActual;

/* RUTINAS SEMANTICAS */

REG_EXPRESION ProcesarCte(void)
{
    /* Convierte cadena que representa numero a numero entero y construye un registro semantico */
    REG_EXPRESION t;
    t.clase = CONSTANTE;
    strcpy(t.nombre, buffer);
    sscanf(buffer, "%d", &t.valor);
    return t;
}

REG_EXPRESION ProcesarId(void)
{
    /* Declara ID y construye el correspondiente registro semantico */
    REG_EXPRESION t;
    Chequear(buffer);
    t.clase = ID;
    strcpy(t.nombre, buffer);
    return t;
}

REG_OPERACION ProcesarOp(void)
{
    /* Declara OP y construye el correspondiente registro semantico */
    REG_OPERACION op;
	op.operador = buffer[0]; /* '+' o '-' */
	op.clase = tokenActual; /* SUMA o RESTA*/
    return op;
}

void Leer(REG_EXPRESION in)
{
    Generar("Read", in.nombre, "Entera", "");
}

void Escribir(REG_EXPRESION out)
{
    Generar("Write", Extraer(&out), "Entera", "");
}

REG_EXPRESION GenInfijo(REG_EXPRESION e1, REG_OPERACION op, REG_EXPRESION e2)
{
	/* Genera la instruccion para una operacion infija y construye un registro semantico con el resultado */
    REG_EXPRESION reg;
    static unsigned int numTemp = 1;
    char cadTemp[TAMLEX+1] ="Temp&";
    char cadNum[TAMLEX+1];
    char cadOp[TAMLEX+1];
    if ( op.clase == SUMA ) strcpy(cadOp, "Suma");
    if ( op.clase == RESTA ) strcpy(cadOp, "Resta");
    sprintf(cadNum, "%d", numTemp);
    numTemp++;
    strcat(cadTemp, cadNum);
    if ( e1.clase == ID) Chequear(Extraer(&e1));
    if ( e2.clase == ID) Chequear(Extraer(&e2));
    Chequear(cadTemp);
    Generar(cadOp, Extraer(&e1), Extraer(&e2), cadTemp);
    strcpy(reg.nombre, cadTemp);
    return reg;
}

/* PROCEDIMIENTOS DE ANALISIS SINTACTICO (PAS) */

void Objetivo(void)
{
    /* <objetivo> -> <programa> FDT #terminar */
    Programa();
    Match(FDT);
    Terminar();
}

void Programa(void)
{
    /* <programa> -> #comenzar INICIO <listaSentencias> FIN */
    Comenzar();
    Match(INICIO);
    ListaSentencias();
    Match(FIN);
}

void ListaSentencias(void)
{
    /* <listaSentencias> -> <sentencia> {<sentencia>} */
    Sentencia();
    while ( 1 )
    {
        switch ( ProximoToken() )
        {
        case ID : case LEER : case ESCRIBIR :
            Sentencia();
            break;
        default :
			return; /* fin switch */
        }
    }
}

void Sentencia(void)
{
    TOKEN tok = ProximoToken();
    REG_EXPRESION izq,der;
    switch ( tok )
    {
        case ID :
            /* <sentencia>-> <identificador> := <expresion> #asignar; */
            Identificador(&izq);
            Match(ASIGNACION);
            Expresion(&der);
            Asignar(izq,der);
            Match(PUNTOYCOMA);
            break;
        case LEER :
            /* <sentencia> -> LEER ( <listaIdentificadores> ); */
            Match(LEER);
            Match(PARENIZQUIERDO);
            ListaIdentificadores();
            Match(PARENDERECHO);
            Match(PUNTOYCOMA);
            break;
        case ESCRIBIR :
            /* <sentencia> -> ESCRIBIR ( <listaExpresiones> ); */
            Match(ESCRIBIR);
            Match(PARENIZQUIERDO);
            ListaExpresiones();
            Match(PARENDERECHO);
            Match(PUNTOYCOMA);
            break;
        default :
	    ErrorSintactico();
            break;
    }
}

void ListaIdentificadores(void)
{
	/* <listaIdentificadores> -> <identificador> #leer_id {COMA <identificador> #leer_id} */
    REG_EXPRESION reg;
    Identificador(&reg);
    Leer(reg);
    while ( 1 )
    {
        switch ( ProximoToken() )
		{
			case COMA:
				Match(COMA);
				Identificador(&reg);
				Leer(reg);
				break;
			default :
				return; /* fin switch */
        }
    }
}

void ListaExpresiones(void)
{
	/* <listaExpresiones> -> <expresion> #escribir_exp {COMA <expresion> #escribir_exp} */
    REG_EXPRESION reg;
    Expresion(&reg);
    Escribir(reg);
    while ( 1 )
    {
        switch ( ProximoToken() )
        {
			case COMA:
				Match(COMA);
				Expresion(&reg);
				Escribir(reg);
				break;
			default :
				return; /* fin switch */
        }
    }
}

void Expresion(REG_EXPRESION * resultado)
{
    /* <expresion> -> <primaria> { <operadorAditivo> <primaria> #gen_infijo} */
    REG_EXPRESION operandoIzq,operandoDer;
    REG_OPERACION op;
    TOKEN t;
    Primaria(&operandoIzq);
    for ( t = ProximoToken(); t == SUMA || t == RESTA; t = ProximoToken() )
    {
        OperadorAditivo(&op);
        Primaria(&operandoDer);
        operandoIzq = GenInfijo(operandoIzq, op, operandoDer);
    }
    *resultado = operandoIzq;
}

void Primaria(REG_EXPRESION * resultado)
{
    TOKEN tok = ProximoToken();
    switch ( tok )
    {
        case ID :
            /* <primaria> -> <identificador> */
            Identificador(resultado);
            break;
        case CONSTANTE :
            /* <primaria> -> CONSTANTE #procesar_cte */
            Match(CONSTANTE);
            *resultado = ProcesarCte();
            break;
        case PARENIZQUIERDO :
            /* <primaria> -> PARENIZQUIERDO <expresion> PARENDERECHO */
            Match(PARENIZQUIERDO);
            Expresion(resultado);
            Match(PARENDERECHO);
            break;
        default :
            ErrorSintactico();
            break;
    }
}

void OperadorAditivo(REG_OPERACION * op)
{
    /* <operadorAditivo> -> SUMA #procesar_op | RESTA #procesar_op */
    TOKEN t = ProximoToken();
    if ( t == SUMA || t == RESTA )
    {
        Match(t);
        *op = ProcesarOp();
    }
    else
        ErrorSintactico();
}

void Identificador(REG_EXPRESION * resultado)
{
    /* <identificador> -> ID #procesar_id */
    Match(ID);
    *resultado = ProcesarId();
}

FILE * in;
char buffer[];

/*SCANNER*/

/*Recibe un caracter y devuelve su valor numerico*/
int columna(int c){
	if ( isalpha(c) ) return 0;
	if ( isdigit(c) ) return 1;
	if ( c == '+' ) return 2;
	if ( c == '-' ) return 3;
	if ( c == '(' ) return 4;
	if ( c == ')' ) return 5;
	if ( c == ',' ) return 6;
	if ( c == ';' ) return 7;
	if ( c == ':' ) return 8;
	if ( c == '=' ) return 9;
	if ( c == EOF ) return 10;
	if ( isspace(c) ) return 11;/* isspace devuelve 1 si c es un espacio*/
	return 12;
}

/*recibe un estado devuelve V si es estado final o F si no lo es*/
int estadoFinal(int e){
	if ( e == 0 || e == 1 || e == 3 || e == 11 || e == 14 ) return 0; /*estados no finales*/
	return 1; /*el resto son finales*/
}

/*Lee caracteres desde el archivo fuente y devuelve el token proximo que encuentre*/
TOKEN scanner(){
	int tabla[NUMEROESTADOS][NUMEROCOLUMNAS] = {
/*    L  D  +  -  (  )  ,   ;   :  =  fdt sp otro*/
	{ 1, 3, 5, 6, 7, 8, 9, 10, 11, 14, 13, 0, 14 },
	{ 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 },
	{ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 },
	{ 4, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 },
	{ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 },
	{ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 },
	{ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 },
	{ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 },
	{ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 },
	{ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 },
	{ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 },
	{ 14, 14, 14, 14, 14, 14, 14, 14, 14, 12, 14, 14, 14 },
	{ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 },
	{ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 },
	{ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 },
	};

	int estado = 0;
	int i = 0;
	char caracter;
	int col;

	/*obtiene el siguiente caracter del archivo, si no es espacio lo agrega al buffer*/
	do{

		caracter = fgetc(in);
		col = columna(caracter);
		estado = tabla[estado][col]; /*me muevo al siguiente estado*/

		if ( col != 11 ) {
			buffer[i] = caracter;
			i++;
		}
	} while ( !estadoFinal(estado) && !(estado == 14) ); /*mientras no se llegue a un estado final o al estado de rechazo*/

	/* una vez terminado cierra la cadena con el nulo*/
	buffer[i] = '\0';

	/*dependiendo el estado final realizo la accion correspondiente*/

	switch ( estado ){

		case 2 :
				/*para cadenas alfanumericas*/
				if ( col != 11 )
					{ /*si lo ultimo leido no fue un espacio
						ungetc 'rebobina' el archivo un caracter*/
					ungetc(caracter, in);
					/*trunco la cadena quitando el caracter extra leido del buffer*/
					buffer[i-1] = '\0';
					}

				return ID;
				break;
		case 4 :
				/*para numeros es similar*/
				if ( col != 11 )
					{
					ungetc(caracter, in);
					buffer[i-1] = '\0';
					}

				return CONSTANTE;
				break;
		case 5 : return SUMA; break;
		case 6 : return RESTA; break;
		case 7 : return PARENIZQUIERDO; break;
		case 8 : return PARENDERECHO; break;
		case 9 : return COMA; break;
		case 10 : return PUNTOYCOMA; break;
		case 12 : return ASIGNACION; break;
		case 13 : return FDT; break;
		case 14 : return ERRORLEXICO; break;
	}

	return 0;
}
