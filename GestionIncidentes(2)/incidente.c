/* ============================================================================
 * ARCHIVO:   incidente.c
 * MODULO:    Gestion de Incidentes (Grupo 1)
 * PROPOSITO: Implementa toda la logica declarada en incidente.h:
 *            - Manejo de la lista doblemente enlazada (crear, insertar,
 *              buscar, modificar, listar, eliminar, liberar memoria).
 *            - Persistencia de datos en el archivo de texto incidentes.txt.
 *            - Validacion basica de los datos ingresados por teclado.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "incidente.h"

/* ---------------------------------------------------------------------------
 * FUNCION AUXILIAR (uso interno): leerLinea
 * Lee una linea de texto con fgets, quita el salto de linea final y NO
 * permite que el campo quede vacio (validacion de datos obligatoria).
 * ------------------------------------------------------------------------- */
static void leerLinea(const char *mensaje, char *destino, int tamanio) {
    int valido = 0;
    do {
        printf("%s", mensaje);
        fgets(destino, tamanio, stdin);
        destino[strcspn(destino, "\n")] = '\0'; /* quitar '\n' final */

        if (strlen(destino) == 0) {
            printf("  >> Este campo no puede quedar vacio. Intente de nuevo.\n");
            valido = 0;
        } else {
            valido = 1;
        }
    } while (!valido);
}

/* ---------------------------------------------------------------------------
 * FUNCION AUXILIAR (uso interno): leerEntero
 * Pide un numero entero dentro de un rango [min, max] y no deja pasar
 * texto invalido (validacion de datos obligatoria).
 * ------------------------------------------------------------------------- */
static int leerEntero(const char *mensaje, int min, int max) {
    char buffer[50];
    int valor, esValido;

    do {
        printf("%s", mensaje);
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        esValido = (strlen(buffer) > 0);
        for (int i = 0; buffer[i] != '\0'; i++) {
            if (!isdigit((unsigned char)buffer[i])) { esValido = 0; break; }
        }

        if (!esValido) {
            printf("  >> Debe ingresar solo numeros.\n");
            continue;
        }

        valor = atoi(buffer);
        if (valor < min || valor > max) {
            printf("  >> El valor debe estar entre %d y %d.\n", min, max);
            esValido = 0;
        }
    } while (!esValido);

    return valor;
}

/* ---------------------------------------------------------------------------
 * inicializarLista
 * Deja la lista vacia (sin nodos) y el contador de codigos en 1.
 * ------------------------------------------------------------------------- */
void inicializarLista(ListaIncidentes *lista) {
    lista->cabeza = NULL;
    lista->cola = NULL;
    lista->totalIncidentes = 0;
    lista->siguienteCodigo = 1;
}

/* ---------------------------------------------------------------------------
 * registrarIncidente
 * Pide todos los datos por teclado (con validacion), reserva memoria con
 * malloc() para el nuevo nodo, lo llena y lo enlaza al FINAL de la lista
 * doblemente enlazada actualizando los punteros *ant y *sig.
 * ------------------------------------------------------------------------- */
void registrarIncidente(ListaIncidentes *lista) {
    /* 1) Reservamos memoria dinamica para el nuevo incidente */
    Incidente *nuevo = (Incidente *)malloc(sizeof(Incidente));
    if (nuevo == NULL) {
        printf("ERROR: no hay memoria suficiente para crear el incidente.\n");
        return;
    }

    printf("\n--- REGISTRAR NUEVO INCIDENTE ---\n");

    /* 2) Pedimos los datos validando que no vengan vacios */
    leerLinea("Fecha (dd/mm/aaaa): ", nuevo->fecha, TAM_FECHA);
    leerLinea("Usuario que reporta: ", nuevo->usuario, TAM_USUARIO);
    leerLinea("Departamento: ", nuevo->departamento, TAM_DEPARTAMENTO);
    leerLinea("Descripcion del incidente: ", nuevo->descripcion, TAM_DESCRIPCION);

    printf("Prioridad -> 1.Baja  2.Media  3.Alta  4.Critica\n");
    nuevo->prioridad = (Prioridad)leerEntero("Seleccione prioridad: ", 1, 4);

    /* 3) El codigo se asigna automaticamente y el estado inicial siempre
          es "Abierto" (regla de negocio: todo incidente nace abierto).   */
    nuevo->codigo = lista->siguienteCodigo;
    strcpy(nuevo->estado, "Abierto");

    /* 4) Como es un nodo nuevo y suelto, sus punteros aun no apuntan a nada */
    nuevo->ant = NULL;
    nuevo->sig = NULL;

    /* 5) Enlazamos el nuevo nodo al FINAL de la lista doblemente enlazada */
    if (lista->cabeza == NULL) {
        /* La lista esta vacia: el nuevo nodo es cabeza y cola a la vez */
        lista->cabeza = nuevo;
        lista->cola = nuevo;
    } else {
        /* Enlazamos el nuevo nodo despues de la cola actual */
        nuevo->ant = lista->cola;    /* el nuevo mira hacia atras, a la cola vieja */
        lista->cola->sig = nuevo;    /* la cola vieja ahora apunta hacia adelante al nuevo */
        lista->cola = nuevo;         /* el nuevo pasa a ser la nueva cola */
    }

    lista->totalIncidentes++;
    lista->siguienteCodigo++;

    printf("\n>> Incidente registrado con codigo #%d (estado: Abierto)\n", nuevo->codigo);

    /* Guardamos de inmediato en el archivo para no perder el registro */
    guardarEnArchivo(lista);
}

/* ---------------------------------------------------------------------------
 * buscarPorCodigo
 * Recorre la lista desde la cabeza usando el puntero *sig hasta encontrar
 * el codigo buscado, o hasta llegar al final (NULL).
 * ------------------------------------------------------------------------- */
Incidente *buscarPorCodigo(ListaIncidentes *lista, int codigo) {
    Incidente *actual = lista->cabeza;
    while (actual != NULL) {
        if (actual->codigo == codigo) {
            return actual;
        }
        actual = actual->sig; /* avanzamos al siguiente nodo */
    }
    return NULL; /* no se encontro ningun incidente con ese codigo */
}

/* ---------------------------------------------------------------------------
 * modificarEstado
 * Pide un codigo, verifica que exista, y luego pide el nuevo estado
 * (menu numerico para evitar errores de digitacion) y lo aplica.
 * ------------------------------------------------------------------------- */
void modificarEstado(ListaIncidentes *lista) {
    int codigo = leerEntero("\nIngrese el codigo del incidente a modificar: ", 1, 999999);
    Incidente *inc = buscarPorCodigo(lista, codigo);

    if (inc == NULL) {
        printf(">> No existe un incidente con el codigo %d.\n", codigo);
        return;
    }

    printf("Estado actual: %s\n", inc->estado);
    printf("Nuevo estado -> 1.Abierto  2.En Proceso  3.Resuelto\n");
    int opcion = leerEntero("Seleccione: ", 1, 3);

    switch (opcion) {
        case 1: strcpy(inc->estado, "Abierto");    break;
        case 2: strcpy(inc->estado, "En Proceso"); break;
        case 3: strcpy(inc->estado, "Resuelto");   break;
    }

    printf(">> Estado actualizado correctamente a '%s'.\n", inc->estado);
    guardarEnArchivo(lista);
}

/* ---------------------------------------------------------------------------
 * listarAbiertos
 * Recorre toda la lista e imprime unicamente los incidentes cuyo campo
 * estado sea exactamente "Abierto".
 * ------------------------------------------------------------------------- */
void listarAbiertos(ListaIncidentes *lista) {
    Incidente *actual = lista->cabeza;
    int encontrados = 0;

    printf("\n--- INCIDENTES ABIERTOS ---\n");
    while (actual != NULL) {
        if (strcmp(actual->estado, "Abierto") == 0) {
            imprimirIncidente(actual);
            encontrados++;
        }
        actual = actual->sig;
    }

    if (encontrados == 0) {
        printf("No hay incidentes abiertos actualmente.\n");
    }
}

/* ---------------------------------------------------------------------------
 * listarPorPrioridad
 * Pide una prioridad al usuario y muestra todos los incidentes que la
 * tengan asignada.
 * ------------------------------------------------------------------------- */
void listarPorPrioridad(ListaIncidentes *lista) {
    printf("Prioridad -> 1.Baja  2.Media  3.Alta  4.Critica\n");
    Prioridad prioridad = (Prioridad)leerEntero("Seleccione prioridad a filtrar: ", 1, 4);

    Incidente *actual = lista->cabeza;
    int encontrados = 0;

    printf("\n--- INCIDENTES CON PRIORIDAD %s ---\n", prioridadATexto(prioridad));
    while (actual != NULL) {
        if (actual->prioridad == prioridad) {
            imprimirIncidente(actual);
            encontrados++;
        }
        actual = actual->sig;
    }

    if (encontrados == 0) {
        printf("No hay incidentes con esa prioridad.\n");
    }
}

/* ---------------------------------------------------------------------------
 * eliminarIncidente
 * Pide un codigo y elimina el nodo SOLO si su estado es "Resuelto"
 * (regla de negocio obligatoria). Al eliminar, reconecta los punteros
 * *ant y *sig de los nodos vecinos para no romper la lista, y libera
 * la memoria del nodo eliminado con free().
 * ------------------------------------------------------------------------- */
void eliminarIncidente(ListaIncidentes *lista) {
    int codigo = leerEntero("\nIngrese el codigo del incidente a eliminar: ", 1, 999999);
    Incidente *inc = buscarPorCodigo(lista, codigo);

    if (inc == NULL) {
        printf(">> No existe un incidente con el codigo %d.\n", codigo);
        return;
    }

    if (strcmp(inc->estado, "Resuelto") != 0) {
        printf(">> Solo se pueden eliminar incidentes en estado 'Resuelto'.\n");
        printf(">> El incidente #%d esta actualmente en estado '%s'.\n", codigo, inc->estado);
        return;
    }

    /* Reconectamos el puntero *sig del nodo anterior (si existe) */
    if (inc->ant != NULL) {
        inc->ant->sig = inc->sig;
    } else {
        /* inc era la cabeza: la nueva cabeza es el siguiente nodo */
        lista->cabeza = inc->sig;
    }

    /* Reconectamos el puntero *ant del nodo siguiente (si existe) */
    if (inc->sig != NULL) {
        inc->sig->ant = inc->ant;
    } else {
        /* inc era la cola: la nueva cola es el nodo anterior */
        lista->cola = inc->ant;
    }

    free(inc); /* liberamos la memoria reservada con malloc */
    lista->totalIncidentes--;

    printf(">> Incidente #%d eliminado correctamente.\n", codigo);
    guardarEnArchivo(lista);
}

/* ---------------------------------------------------------------------------
 * imprimirIncidente
 * Muestra en pantalla todos los campos de un incidente con formato claro.
 * ------------------------------------------------------------------------- */
void imprimirIncidente(Incidente *inc) {
    printf("--------------------------------------------------\n");
    printf(" Codigo       : %d\n", inc->codigo);
    printf(" Fecha        : %s\n", inc->fecha);
    printf(" Usuario      : %s\n", inc->usuario);
    printf(" Departamento : %s\n", inc->departamento);
    printf(" Descripcion  : %s\n", inc->descripcion);
    printf(" Prioridad    : %s\n", prioridadATexto(inc->prioridad));
    printf(" Estado       : %s\n", inc->estado);
    printf("--------------------------------------------------\n");
}

/* ---------------------------------------------------------------------------
 * guardarEnArchivo
 * Recorre la lista completa (desde la cabeza, siguiendo *sig) y escribe
 * una linea por cada incidente en incidentes.txt. Se abre en modo "w",
 * por lo que el archivo se sobrescribe con el estado actual de la lista
 * cada vez que se llama a esta funcion.
 * Formato de cada linea (separado por '|'):
 *   codigo|fecha|usuario|departamento|descripcion|prioridad|estado
 * ------------------------------------------------------------------------- */
void guardarEnArchivo(ListaIncidentes *lista) {
    FILE *archivo = fopen(NOMBRE_ARCHIVO, "w");
    if (archivo == NULL) {
        printf("ERROR: no se pudo abrir '%s' para guardar los datos.\n", NOMBRE_ARCHIVO);
        return;
    }

    Incidente *actual = lista->cabeza;
    while (actual != NULL) {
        fprintf(archivo, "%d|%s|%s|%s|%s|%s|%s\n",
                actual->codigo,
                actual->fecha,
                actual->usuario,
                actual->departamento,
                actual->descripcion,
                prioridadATexto(actual->prioridad),
                actual->estado);
        actual = actual->sig;
    }

    fclose(archivo);
}

/* ---------------------------------------------------------------------------
 * cargarDesdeArchivo
 * Se ejecuta al iniciar el programa (main). Lee incidentes.txt linea por
 * linea y reconstruye la lista doblemente enlazada en memoria usando
 * strtok() para separar los campos por el delimitador '|'.
 * Si el archivo no existe (primera ejecucion), simplemente no hace nada
 * y el programa continua con la lista vacia.
 * ------------------------------------------------------------------------- */
void cargarDesdeArchivo(ListaIncidentes *lista) {
    FILE *archivo = fopen(NOMBRE_ARCHIVO, "r");
    if (archivo == NULL) {
        printf("Aviso: no existe '%s' todavia. Se inicia con la lista vacia.\n", NOMBRE_ARCHIVO);
        return;
    }

    char linea[400];
    int codigoMaximo = 0;

    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        linea[strcspn(linea, "\n")] = '\0';
        if (strlen(linea) == 0) continue;

        /* Reservamos memoria para el nodo que vamos a reconstruir */
        Incidente *nuevo = (Incidente *)malloc(sizeof(Incidente));
        if (nuevo == NULL) continue;

        char *token;
        char prioridadTxt[15];

        token = strtok(linea, "|");
        if (token == NULL) { free(nuevo); continue; }
        nuevo->codigo = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) { free(nuevo); continue; }
        strncpy(nuevo->fecha, token, TAM_FECHA - 1);
        nuevo->fecha[TAM_FECHA - 1] = '\0';

        token = strtok(NULL, "|");
        if (token == NULL) { free(nuevo); continue; }
        strncpy(nuevo->usuario, token, TAM_USUARIO - 1);
        nuevo->usuario[TAM_USUARIO - 1] = '\0';

        token = strtok(NULL, "|");
        if (token == NULL) { free(nuevo); continue; }
        strncpy(nuevo->departamento, token, TAM_DEPARTAMENTO - 1);
        nuevo->departamento[TAM_DEPARTAMENTO - 1] = '\0';

        token = strtok(NULL, "|");
        if (token == NULL) { free(nuevo); continue; }
        strncpy(nuevo->descripcion, token, TAM_DESCRIPCION - 1);
        nuevo->descripcion[TAM_DESCRIPCION - 1] = '\0';

        token = strtok(NULL, "|");
        if (token == NULL) { free(nuevo); continue; }
        strncpy(prioridadTxt, token, 14);
        prioridadTxt[14] = '\0';
        nuevo->prioridad = textoAPrioridad(prioridadTxt);

        token = strtok(NULL, "|");
        if (token == NULL) { free(nuevo); continue; }
        strncpy(nuevo->estado, token, TAM_ESTADO - 1);
        nuevo->estado[TAM_ESTADO - 1] = '\0';

        nuevo->ant = NULL;
        nuevo->sig = NULL;

        /* Insertamos el nodo reconstruido al final de la lista */
        if (lista->cabeza == NULL) {
            lista->cabeza = nuevo;
            lista->cola = nuevo;
        } else {
            nuevo->ant = lista->cola;
            lista->cola->sig = nuevo;
            lista->cola = nuevo;
        }
        lista->totalIncidentes++;

        if (nuevo->codigo > codigoMaximo) {
            codigoMaximo = nuevo->codigo;
        }
    }

    fclose(archivo);

    /* El siguiente codigo a usar debe continuar despues del mayor cargado */
    lista->siguienteCodigo = codigoMaximo + 1;

    printf("Se cargaron %d incidente(s) desde '%s'.\n", lista->totalIncidentes, NOMBRE_ARCHIVO);
}

/* ---------------------------------------------------------------------------
 * liberarLista
 * Recorre toda la lista liberando cada nodo con free() para evitar fugas
 * de memoria. Se debe llamar justo antes de terminar el programa.
 * ------------------------------------------------------------------------- */
void liberarLista(ListaIncidentes *lista) {
    Incidente *actual = lista->cabeza;
    Incidente *siguienteTemp;

    while (actual != NULL) {
        siguienteTemp = actual->sig; /* guardamos referencia antes de liberar */
        free(actual);
        actual = siguienteTemp;
    }

    lista->cabeza = NULL;
    lista->cola = NULL;
    lista->totalIncidentes = 0;
}

/* ---------------------------------------------------------------------------
 * prioridadATexto / textoAPrioridad
 * Funciones de conversion entre el enum Prioridad y su representacion
 * en texto, usadas tanto para mostrar en pantalla como para guardar y
 * leer del archivo de texto.
 * ------------------------------------------------------------------------- */
const char *prioridadATexto(Prioridad p) {
    switch (p) {
        case BAJA:    return "Baja";
        case MEDIA:   return "Media";
        case ALTA:    return "Alta";
        case CRITICA: return "Critica";
        default:      return "Desconocida";
    }
}

Prioridad textoAPrioridad(const char *texto) {
    if (strcmp(texto, "Baja") == 0)    return BAJA;
    if (strcmp(texto, "Media") == 0)   return MEDIA;
    if (strcmp(texto, "Alta") == 0)    return ALTA;
    if (strcmp(texto, "Critica") == 0) return CRITICA;
    return BAJA; /* valor por defecto si el texto no coincide */
}