#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef long long i64;
typedef __int128 i128;

// Constante para representar -∞
const i64 NEG_INF = (i64)-4e18;

/*
 * Estructura ConvexHull: mantiene un conjunto de líneas (pendiente, intersección)
 * y permite consultar el máximo valor para una x dada.
 * Implementa Convex Hull Trick optimizado para consultas monótonas.
 */
typedef struct {
    i64 *pendientes;          // Array de pendientes (m)
    i64 *intersecciones;      // Array de intersecciones (b)
    int cantidad;             // Cantidad actual de líneas
    int capacidad;            // Capacidad del array
    int indice_actual;        // Puntero al mejor índice para consultas monótonas
} ConvexHull;

// Inicializa una estructura ConvexHull
ConvexHull* crearConvexHull(int capacidad_inicial) {
    ConvexHull *hull = (ConvexHull *)malloc(sizeof(ConvexHull));
    hull->pendientes = (i64 *)malloc(capacidad_inicial * sizeof(i64));
    hull->intersecciones = (i64 *)malloc(capacidad_inicial * sizeof(i64));
    hull->cantidad = 0;
    hull->capacidad = capacidad_inicial;
    hull->indice_actual = 0;
    return hull;
}

// Libera memoria de ConvexHull
void destruirConvexHull(ConvexHull *hull) {
    if (hull) {
        free(hull->pendientes);
        free(hull->intersecciones);
        free(hull);
    }
}

// Duplica la capacidad del array si es necesario
void redimensionarConvexHull(ConvexHull *hull) {
    hull->capacidad *= 2;
    hull->pendientes = (i64 *)realloc(hull->pendientes, hull->capacidad * sizeof(i64));
    hull->intersecciones = (i64 *)realloc(hull->intersecciones, hull->capacidad * sizeof(i64));
}

/*
 * Determina si la línea 2 es innecesaria (no contribuye al máximo).
 * Compara tres líneas consecutivas para validar convexidad.
 */
int esRedundante(i64 m1, i64 b1, i64 m2, i64 b2, i64 m3, i64 b3) {
    i128 lado_izq = (i128)(b3 - b1) * (m1 - m2);
    i128 lado_der = (i128)(b2 - b1) * (m1 - m3);
    return lado_izq <= lado_der;
}

// Evalúa la función f(x) = m*x + b en el índice dado
i64 evaluar(const ConvexHull *hull, int indice, i64 x) {
    return hull->pendientes[indice] * x + hull->intersecciones[indice];
}

/*
 * Agrega una nueva línea con pendiente 'm' e intersección 'b'.
 * Elimina líneas redundantes del final del hull.
 */
void agregarLinea(ConvexHull *hull, i64 m, i64 b) {
    // Elimina líneas redundantes del final
    while (hull->cantidad >= 2 && esRedundante(
            hull->pendientes[hull->cantidad - 2], 
            hull->intersecciones[hull->cantidad - 2],
            hull->pendientes[hull->cantidad - 1], 
            hull->intersecciones[hull->cantidad - 1],
            m, b)) {
        hull->cantidad--;
        if (hull->indice_actual > hull->cantidad - 1)
            hull->indice_actual = hull->cantidad - 1;
    }
    
    // Redimensiona si es necesario
    if (hull->cantidad >= hull->capacidad)
        redimensionarConvexHull(hull);
    
    // Agrega la nueva línea
    hull->pendientes[hull->cantidad] = m;
    hull->intersecciones[hull->cantidad] = b;
    hull->cantidad++;
    
    if (hull->indice_actual >= hull->cantidad)
        hull->indice_actual = hull->cantidad - 1;
}

/*
 * Consulta el máximo valor para un x dado.
 * Usa búsqueda lineal optimizada con puntero monótono.
 */
i64 consultar(ConvexHull *hull, i64 x) {
    if (hull->cantidad == 0)
        return NEG_INF;
    
    // Avanza el puntero mientras la siguiente línea sea mejor
    while (hull->cantidad - hull->indice_actual >= 2 && 
           evaluar(hull, hull->indice_actual, x) <= 
           evaluar(hull, hull->indice_actual + 1, x)) {
        hull->indice_actual++;
    }
    
    return evaluar(hull, hull->indice_actual, x);
}

// Calcula X(t) = -2 * gap_score * t
i64 calcularX(i64 gap_score, i64 t) {
    return (-2LL * gap_score) * t;
}

int main() {
    int longitud_A, longitud_B;
    char *secuencia_A, *secuencia_B;
    i64 score_acierto, score_error, score_brecha;
    
    // Lee entrada
    if (scanf("%d", &longitud_A) != 1)
        return 0;
    
    secuencia_A = (char *)malloc((longitud_A + 1) * sizeof(char));
    if (scanf("%s", secuencia_A) != 1) {
        free(secuencia_A);
        return 0;
    }
    
    if (scanf("%d", &longitud_B) != 1) {
        free(secuencia_A);
        return 0;
    }
    
    secuencia_B = (char *)malloc((longitud_B + 1) * sizeof(char));
    if (scanf("%s", secuencia_B) != 1) {
        free(secuencia_A);
        free(secuencia_B);
        return 0;
    }
    
    if (scanf("%lld %lld %lld", &score_acierto, &score_error, &score_brecha) != 3) {
        free(secuencia_A);
        free(secuencia_B);
        return 0;
    }
    
    // Tablas DP
    i64 *alineacion_prev = (i64 *)malloc((longitud_B + 1) * sizeof(i64));
    i64 *alineacion_cur = (i64 *)malloc((longitud_B + 1) * sizeof(i64));
    i64 *brecha_A_prev = (i64 *)malloc((longitud_B + 1) * sizeof(i64));
    i64 *brecha_A_cur = (i64 *)malloc((longitud_B + 1) * sizeof(i64));
    i64 *brecha_B_prev = (i64 *)malloc((longitud_B + 1) * sizeof(i64));
    i64 *brecha_B_cur = (i64 *)malloc((longitud_B + 1) * sizeof(i64));
    
    ConvexHull **hull_columnas = (ConvexHull **)malloc((longitud_B + 1) * sizeof(ConvexHull *));
    for (int j = 0; j <= longitud_B; j++)
        hull_columnas[j] = crearConvexHull(longitud_A + 10);
    
    // Inicialización base
    alineacion_prev[0] = 0;
    for (int j = 1; j <= longitud_B; j++) {
        alineacion_prev[j] = NEG_INF;
        brecha_B_prev[j] = NEG_INF;
        brecha_A_prev[j] = score_brecha * j * j;
    }
    
    // Inicializa los convex hulls por columna
    for (int j = 0; j <= longitud_B; j++) {
        i64 base = (alineacion_prev[j] > brecha_A_prev[j]) ? alineacion_prev[j] : brecha_A_prev[j];
        agregarLinea(hull_columnas[j], 0, base);
    }
    
    // Iteración principal sobre la secuencia A
    for (int i = 1; i <= longitud_A; i++) {
        i64 costo_brecha_i = score_brecha * i * i;
        i64 x_i = calcularX(score_brecha, i);
        
        // Calcula brechas verticales (GapB)
        for (int j = 0; j <= longitud_B; j++) {
            brecha_B_cur[j] = consultar(hull_columnas[j], x_i);
            if (brecha_B_cur[j] != NEG_INF)
                brecha_B_cur[j] += costo_brecha_i;
        }
        brecha_B_cur[0] = costo_brecha_i;
        
        // Calcula coincidencias o desajustes
        alineacion_cur[0] = NEG_INF;
        for (int j = 1; j <= longitud_B; j++) {
            i64 score = (secuencia_A[i - 1] == secuencia_B[j - 1]) ? score_acierto : score_error;
            i64 mejor_anterior = alineacion_prev[j - 1];
            if (brecha_A_prev[j - 1] > mejor_anterior)
                mejor_anterior = brecha_A_prev[j - 1];
            if (brecha_B_prev[j - 1] > mejor_anterior)
                mejor_anterior = brecha_B_prev[j - 1];
            
            alineacion_cur[j] = (mejor_anterior == NEG_INF) ? NEG_INF : (mejor_anterior + score);
        }
        
        // Calcula brechas horizontales (GapA)
        ConvexHull *hull_fila = crearConvexHull(longitud_B + 10);
        {
            i64 base = (alineacion_cur[0] > brecha_B_cur[0]) ? alineacion_cur[0] : brecha_B_cur[0];
            agregarLinea(hull_fila, 0, base);
        }
        
        brecha_A_cur[0] = NEG_INF;
        for (int j = 1; j <= longitud_B; j++) {
            i64 val = consultar(hull_fila, calcularX(score_brecha, j));
            brecha_A_cur[j] = (val == NEG_INF) ? NEG_INF : (val + score_brecha * j * j);
            i64 base = (alineacion_cur[j] > brecha_B_cur[j]) ? alineacion_cur[j] : brecha_B_cur[j];
            agregarLinea(hull_fila, j, base + score_brecha * j * j);
        }
        
        destruirConvexHull(hull_fila);
        
        // Actualiza convex hulls por columnas
        for (int j = 0; j <= longitud_B; j++) {
            i64 base = (alineacion_cur[j] > brecha_A_cur[j]) ? alineacion_cur[j] : brecha_A_cur[j];
            agregarLinea(hull_columnas[j], i, base + score_brecha * i * i);
        }
        
        // Intercambia filas
        i64 *temp;
        temp = alineacion_prev; alineacion_prev = alineacion_cur; alineacion_cur = temp;
        temp = brecha_A_prev; brecha_A_prev = brecha_A_cur; brecha_A_cur = temp;
        temp = brecha_B_prev; brecha_B_prev = brecha_B_cur; brecha_B_cur = temp;
        
        // Limpia buffers actuales
        memset(alineacion_cur, 0xFF, (longitud_B + 1) * sizeof(i64));
        memset(brecha_A_cur, 0xFF, (longitud_B + 1) * sizeof(i64));
        memset(brecha_B_cur, 0xFF, (longitud_B + 1) * sizeof(i64));
    }
    
    // Resultado final
    i64 resultado = alineacion_prev[longitud_B];
    if (brecha_A_prev[longitud_B] > resultado)
        resultado = brecha_A_prev[longitud_B];
    if (brecha_B_prev[longitud_B] > resultado)
        resultado = brecha_B_prev[longitud_B];
    
    printf("%lld\n", resultado);
    
    // Libera memoria
    free(alineacion_prev);
    free(alineacion_cur);
    free(brecha_A_prev);
    free(brecha_A_cur);
    free(brecha_B_prev);
    free(brecha_B_cur);
    free(secuencia_A);
    free(secuencia_B);
    
    for (int j = 0; j <= longitud_B; j++)
        destruirConvexHull(hull_columnas[j]);
    free(hull_columnas);
    
    return 0;
}