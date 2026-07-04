/* ============================================================================
 * ARCHIVO:   main.c
 * MODULO:    Gestion de Incidentes (Grupo 1)
 * PROYECTO:  Sistema Integral de Gestion de Servicios TI - TechSolutions S.A.
 * PROPOSITO: Punto de entrada del programa. Muestra el menu, recibe la
 *            opcion del usuario y llama a las funciones de incidente.c
 *
 * FUNCIONAMIENTO GENERAL:
 *   1. Al iniciar, se carga automaticamente incidentes.txt (si existe).
 *   2. Se muestra un menu en bucle con las 6 operaciones requeridas.
 *   3. Cada cambio (registrar, modificar, eliminar) se guarda de inmediato
 *      en el archivo de texto dentro de las propias funciones de incidente.c
 *   4. Al elegir "Salir", se guarda una ultima vez y se libera toda la
 *      memoria dinamica reservada durante la ejecucion.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "incidente.h"

/* Muestra el menu principal en pantalla */
static void mostrarMenu(void) {
    printf("\n============================================================\n");
    printf("   SISTEMA DE GESTION DE INCIDENTES - TechSolutions S.A.\n");
    printf("============================================================\n");
    printf(" 1. Registrar incidente\n");
    printf(" 2. Buscar incidente por codigo\n");
    printf(" 3. Modificar estado de un incidente\n");
    printf(" 4. Listar incidentes 'Abiertos'\n");
    printf(" 5. Listar incidentes por prioridad\n");
    printf(" 6. Eliminar incidente (solo si esta 'Resuelto')\n");
    printf(" 0. Salir y guardar\n");
    printf("============================================================\n");
}

/* Lee la opcion del menu validando que sea un numero entre 0 y 6 */
static int leerOpcionMenu(void) {
    char buffer[20];
    int opcion, esValido;

    do {
        printf("Seleccione una opcion: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        esValido = (strlen(buffer) > 0);
        for (int i = 0; buffer[i] != '\0'; i++) {
            if (!isdigit((unsigned char)buffer[i])) { esValido = 0; break; }
        }

        if (esValido) {
            opcion = atoi(buffer);
            if (opcion < 0 || opcion > 6) {
                printf("  >> Opcion fuera de rango (0-6).\n");
                esValido = 0;
            }
        } else {
            printf("  >> Ingrese solo numeros.\n");
        }
    } while (!esValido);

    return opcion;
}

/* Funcion auxiliar para la opcion "Buscar" del menu */
static void opcionBuscar(ListaIncidentes *lista) {
    char buffer[20];
    printf("\nIngrese el codigo a buscar: ");
    fgets(buffer, sizeof(buffer), stdin);
    int codigo = atoi(buffer);

    Incidente *encontrado = buscarPorCodigo(lista, codigo);
    if (encontrado != NULL) {
        printf("\nIncidente encontrado:\n");
        imprimirIncidente(encontrado);
    } else {
        printf("\n>> No existe un incidente con el codigo %d.\n", codigo);
    }
}

/* ---------------------------------------------------------------------------
 * main
 * Bucle principal: carga datos, muestra el menu, ejecuta la opcion
 * elegida y, al salir, guarda y libera toda la memoria.
 * ------------------------------------------------------------------------- */
int main(void) {
    ListaIncidentes lista;
    int opcion;

    inicializarLista(&lista);
    cargarDesdeArchivo(&lista);   /* recupera los datos guardados previamente */

    do {
        mostrarMenu();
        opcion = leerOpcionMenu();

        switch (opcion) {
            case 1:
                registrarIncidente(&lista);
                break;
            case 2:
                opcionBuscar(&lista);
                break;
            case 3:
                modificarEstado(&lista);
                break;
            case 4:
                listarAbiertos(&lista);
                break;
            case 5:
                listarPorPrioridad(&lista);
                break;
            case 6:
                eliminarIncidente(&lista);
                break;
            case 0:
                printf("\nGuardando cambios antes de salir...\n");
                guardarEnArchivo(&lista);
                printf("Datos guardados en '%s'. Hasta pronto.\n", NOMBRE_ARCHIVO);
                break;
        }

    } while (opcion != 0);

    liberarLista(&lista); /* liberamos toda la memoria dinamica reservada */
    return 0;
}