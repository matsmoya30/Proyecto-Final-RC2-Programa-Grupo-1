/* ============================================================================
 * ARCHIVO:   incidente.h
 * MODULO:    Gestion de Incidentes (Grupo 1)
 * PROPOSITO: Definiciones de tipos de datos y prototipos de funciones.
 *            Aqui se declara el struct Incidente (nodo de la lista
 *            doblemente enlazada) y todas las operaciones disponibles
 *            sobre la lista.
 * ============================================================================
 */

#ifndef INCIDENTE_H
#define INCIDENTE_H

/* ---- Tamanios maximos de los campos de texto ---- */
#define TAM_FECHA        11   /* formato dd/mm/aaaa + '\0'               */
#define TAM_USUARIO       50
#define TAM_DEPARTAMENTO  50
#define TAM_DESCRIPCION  200
#define TAM_ESTADO        15  /* "Abierto", "En Proceso", "Resuelto"     */
#define NOMBRE_ARCHIVO   "incidentes.txt"


/* ----------------------------------------------------------------------
 * ENUM Prioridad
 * Representa los 4 niveles de prioridad pedidos: 1-Baja, 2-Media,
 * 3-Alta, 4-Critica. Se usa enum en vez de un simple int para que el
 * codigo sea mas legible (BAJA en vez de "1" suelto en el codigo).
 * ---------------------------------------------------------------------- */
typedef enum {
    BAJA    = 1,
    MEDIA   = 2,
    ALTA    = 3,
    CRITICA = 4
} Prioridad;

/* ----------------------------------------------------------------------
 * STRUCT Incidente  (nodo de la lista doblemente enlazada)
 * Contiene todos los campos solicitados: codigo, fecha, usuario,
 * departamento, descripcion, prioridad y estado (texto).
 *
 * *ant y *sig son los punteros obligatorios que permiten recorrer la
 * lista hacia atras (ant = anterior) y hacia adelante (sig = siguiente).
 * ---------------------------------------------------------------------- */
typedef struct Incidente {
    int  codigo;                          /* Codigo unico autoincremental   */
    char fecha[TAM_FECHA];                 /* dd/mm/aaaa                     */
    char usuario[TAM_USUARIO];             /* Quien reporta el incidente     */
    char departamento[TAM_DEPARTAMENTO];   /* Departamento de origen         */
    char descripcion[TAM_DESCRIPCION];     /* Detalle del incidente          */
    Prioridad prioridad;                   /* BAJA, MEDIA, ALTA o CRITICA    */
    char estado[TAM_ESTADO];               /* "Abierto"/"En Proceso"/"Resuelto" */

    struct Incidente *ant;                 /* Puntero al nodo ANTERIOR       */
    struct Incidente *sig;                 /* Puntero al nodo SIGUIENTE      */
} Incidente;

/* ----------------------------------------------------------------------
 * STRUCT ListaIncidentes
 * Cabecera de la lista doblemente enlazada: guarda el primer nodo
 * (cabeza), el ultimo nodo (cola) y un contador para asignar codigos
 * unicos automaticamente.
 * ---------------------------------------------------------------------- */
typedef struct {
    Incidente *cabeza;
    Incidente *cola;
    int totalIncidentes;
    int siguienteCodigo;   /* siguiente codigo disponible para asignar */
} ListaIncidentes;

/* =====================  PROTOTIPOS DE FUNCIONES  ========================= */

/* Deja la lista vacia y lista para usarse */
void inicializarLista(ListaIncidentes *lista);

/* Pide datos por teclado, crea el nodo y lo agrega al final de la lista.
   El estado inicial siempre es "Abierto" y el codigo se asigna solo.   */
void registrarIncidente(ListaIncidentes *lista);

/* Busca un incidente por su codigo. Devuelve el puntero al nodo o NULL. */
Incidente *buscarPorCodigo(ListaIncidentes *lista, int codigo);

/* Pide un codigo y un nuevo estado, y lo actualiza si el incidente existe. */
void modificarEstado(ListaIncidentes *lista);

/* Imprime todos los incidentes cuyo estado sea "Abierto". */
void listarAbiertos(ListaIncidentes *lista);

/* Pide una prioridad y muestra todos los incidentes que la tengan. */
void listarPorPrioridad(ListaIncidentes *lista);

/* Pide un codigo y elimina el incidente SOLO si su estado es "Resuelto". */
void eliminarIncidente(ListaIncidentes *lista);

/* Recorre la lista e imprime en pantalla los datos de un solo incidente. */
void imprimirIncidente(Incidente *inc);

/* -------------------- Persistencia en archivo de texto ------------------ */

/* Guarda TODOS los incidentes de la lista en incidentes.txt (sobrescribe). */
void guardarEnArchivo(ListaIncidentes *lista);

/* Lee incidentes.txt (si existe) y reconstruye la lista en memoria. */
void cargarDesdeArchivo(ListaIncidentes *lista);

/* Libera toda la memoria dinamica de la lista (usar al cerrar el programa). */
void liberarLista(ListaIncidentes *lista);

/* --------------------- Utilidades de conversion -------------------------- */

/* Convierte el enum Prioridad a texto legible ("Baja", "Media", etc.) */
const char *prioridadATexto(Prioridad p);

/* Convierte un texto ("Baja", "Media", etc.) al enum Prioridad correspondiente */
Prioridad textoAPrioridad(const char *texto);

#endif /* INCIDENTE_H */