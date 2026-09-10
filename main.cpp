#include <iostream>
#include <cmath>
#include <cstring>
#include <cctype>
#include <omp.h>

using namespace std;

class BusquedaExhaustiva {
private:
    const char* caracteres = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const int base = 36;
    char* claveObjetivo;
    int longitud;
    unsigned long long totalCombinaciones;


    void generarCombinacion(unsigned long long indice, char* resultado) {
        for (int i = longitud - 1; i >= 0; --i) {
            resultado[i] = caracteres[indice % base];
            indice /= base;
        }
        resultado[longitud] = '\0';
    }

public:
    BusquedaExhaustiva(const char* clave) {
        longitud = strlen(clave);
        claveObjetivo = new char[longitud + 1];
        strcpy(claveObjetivo, clave);
        totalCombinaciones = pow(base, longitud);
    }

    ~BusquedaExhaustiva() {
        delete[] claveObjetivo;
    }

    static bool validarClave(const char* clave, int longitudEsperada) {
        int len = strlen(clave);
        if (len == 0 || (longitudEsperada > 0 && len != longitudEsperada)) {
            cout << "Error: La longitud debe ser de " << longitudEsperada << " caracteres." << endl;
            return false;
        }
        for (int i = 0; i < len; ++i) {
            if (!isalnum(clave[i]) || islower(clave[i])) {
                cout << "Error: Solo se permiten mayusculas (A-Z) y numeros (0-9). Sin espacios." << endl;
                return false;
            }
        }
        return true;
    }

    double busquedaSecuencial() {
        cout << "\n--- Iniciando Busqueda Secuencial ---" << endl;
        char* intento = new char[longitud + 1];
        bool encontrada = false;
        unsigned long long combinacionesRevisadas = 0;

        double inicioTiempo = omp_get_wtime();

        for (unsigned long long i = 0; i < totalCombinaciones; ++i) {
            generarCombinacion(i, intento);
            combinacionesRevisadas++;

            if (strcmp(intento, claveObjetivo) == 0) {
                encontrada = true;
                break;
            }
        }

        double finTiempo = omp_get_wtime();
        double tiempoTotal = finTiempo - inicioTiempo;

        if (encontrada) {
            cout << "Clave encontrada: " << intento << endl;
        }
        cout << "Combinaciones revisadas: " << combinacionesRevisadas << endl;
        cout << "Tiempo secuencial: " << tiempoTotal << " segundos" << endl;

        delete[] intento;
        return tiempoTotal;
    }

    double busquedaParalela() {
        cout << "\n--- Iniciando Busqueda Paralela ---" << endl;

        int numHilosMax = omp_get_max_threads();


        unsigned long long* inicios = new unsigned long long[numHilosMax];
        unsigned long long* fines = new unsigned long long[numHilosMax];
        unsigned long long* cantidades = new unsigned long long[numHilosMax];
        bool* hilosExito = new bool[numHilosMax];

        for(int i = 0; i < numHilosMax; i++) hilosExito[i] = false;

        bool claveGlobalEncontrada = false;
        int hiloGanador = -1;
        char* claveEncontradaTexto = new char[longitud + 1];

        double inicioTiempo = omp_get_wtime();

        #pragma omp parallel
        {
            int idHilo = omp_get_thread_num();
            int totalHilos = omp_get_num_threads();

            unsigned long long chunk = totalCombinaciones / totalHilos;
            unsigned long long inicio = idHilo * chunk;
            unsigned long long fin = (idHilo == totalHilos - 1) ? totalCombinaciones : inicio + chunk;

            inicios[idHilo] = inicio;
            fines[idHilo] = fin - 1;
            cantidades[idHilo] = fin - inicio;

            char* inicioStr = new char[longitud + 1];
            char* finStr = new char[longitud + 1];
            generarCombinacion(inicio, inicioStr);
            generarCombinacion(fin - 1, finStr);

            #pragma omp critical
            {
                cout << "Hilo " << idHilo << " -> Inicio: " << inicioStr
                     << " -> Fin: " << finStr << " -> Cantidad: " << cantidades[idHilo] << endl;
            }

            char* intento = new char[longitud + 1];

            for (unsigned long long i = inicio; i < fin; ++i) {

                if (claveGlobalEncontrada) break;

                generarCombinacion(i, intento);

                if (strcmp(intento, claveObjetivo) == 0) {
                    #pragma omp critical
                    {
                        if (!claveGlobalEncontrada) {
                            claveGlobalEncontrada = true;
                            hiloGanador = idHilo;
                            hilosExito[idHilo] = true;
                            strcpy(claveEncontradaTexto, intento);
                        }
                    }
                    break;
                }
            }

            delete[] intento;
            delete[] inicioStr;
            delete[] finStr;
        }

        double finTiempo = omp_get_wtime();
        double tiempoTotal = finTiempo - inicioTiempo;

        cout << "\n--- Estado Final de los Hilos ---" << endl;
        for (int i = 0; i < numHilosMax; ++i) {
            cout << "Hilo " << i << ": Rango [" << inicios[i] << " - " << fines[i]
                 << "] | Asignadas: " << cantidades[i]
                 << " | Estado: " << (hilosExito[i] ? "Encontro la clave" : (claveGlobalEncontrada ? "Detenido prematuramente" : "Finalizo sin exito")) << endl;
        }

        if (claveGlobalEncontrada) {
            cout << "\nClave encontrada por el hilo: " << hiloGanador << endl;
            cout << "Clave: " << claveEncontradaTexto << endl;
        }

        cout << "Tiempo paralelo: " << tiempoTotal << " segundos" << endl;

        delete[] inicios;
        delete[] fines;
        delete[] cantidades;
        delete[] hilosExito;
        delete[] claveEncontradaTexto;

        return tiempoTotal;
    }

    void imprimirInformacionGeneral() {
        cout << "\n=== Espacio de Busqueda ===" << endl;
        cout << "Clave objetivo: " << claveObjetivo << endl;
        cout << "Conjunto de caracteres: " << caracteres << " (36)" << endl;
        cout << "Combinaciones totales: " << totalCombinaciones << endl;
    }
};

int main() {
    char inputBuffer[100];
    int longitudDeseada = 6;

    cout << "Ingrese la clave de prueba (" << longitudDeseada << " caracteres, mayusculas/numeros): ";
    cin >> inputBuffer;

    while (!BusquedaExhaustiva::validarClave(inputBuffer, longitudDeseada)) {
        cout << "Ingrese nuevamente la clave: ";
        cin >> inputBuffer;
    }

    BusquedaExhaustiva busqueda(inputBuffer);
    busqueda.imprimirInformacionGeneral();

    double tSecuencial = busqueda.busquedaSecuencial();
    double tParalelo = busqueda.busquedaParalela();

    cout << "\n=== Resumen de Rendimiento ===" << endl;
    cout << "Tiempo Secuencial: " << tSecuencial << " s" << endl;
    cout << "Tiempo Paralelo: " << tParalelo << " s" << endl;

    if (tParalelo > 0) {
        cout << "Speedup (Aceleracion): " << tSecuencial / tParalelo << "x" << endl;
    }

    return 0;
}
