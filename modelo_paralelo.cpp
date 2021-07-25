//
// Created by rudri on 7/24/2021.
//

#include "modelo_paralelo.h"
#include <iostream>
#include <random>
#include <fstream>
#include <numeric>
#include <chrono>
#include <algorithm>
#include <thread>
using namespace std;

void generar_archivo(int n) {
    ofstream file("../datos.txt");
    random_device rd;
    uniform_int_distribution<int> dist(1, 20);
    for (int i = 0; i < n; ++i)
        file << dist(rd) << " ";
}

vector<int> generar_vector() {
    vector<int> result;
    ifstream file("../datos.txt");
    int value;
    while (file >> value)
        result.push_back(value);
    return result;
}

using time_point_t = std::chrono::time_point<std::chrono::high_resolution_clock>;
class timer_t {
    time_point_t start;
    time_point_t stop;
public:
    timer_t() { start = chrono::high_resolution_clock::now(); }
    ~timer_t() {
        stop = chrono::high_resolution_clock::now();
        cout    << "duration: "
                << (chrono::duration_cast<chrono::microseconds>(stop - start).count()) << endl;
    }
};

template<typename Iterator>
void acumulacion(Iterator start, Iterator stop, Iterator result) {
    *result = accumulate(start, stop, 0);
}

void ejemplo_sumarizacion() {
    vector<int> vec;
    {
        timer_t t;
        vec = generar_vector();
    }
    cout << size(vec) << endl;
    {
        timer_t t;
        auto total = accumulate(begin(vec), end(vec), 0); // serial
        cout << "Total: " << total << endl;
    }
//    cout << thread::hardware_concurrency() << endl;

    {
        timer_t t;
        // Cuanto items va a procesar cada hilo;
        int nhilos = static_cast<int>(thread::hardware_concurrency()) * 2;
        int rango = ceil(static_cast<double>(size(vec)) / (nhilos));
        int longitud = 0;

        vector<thread> vhilos (nhilos);
        vector<int> result (nhilos);

        auto current = begin(vec);
        auto stop = end(vec);
        auto iter_resultado = begin(result);
        // Recorrer los hilos
        for (auto& item: vhilos) {
            if (longitud + rango >= size(vec))
                rango += static_cast<int>(size(vec)) - (longitud + rango);
            // El error estaba en que si se envía
            // los valores por referencia en el lambda los rangos van cambiando.
            item = thread(
                    [=]{acumulacion (current, next(current, rango), iter_resultado);});
            current = next(current, rango);
            ++iter_resultado;
            longitud += rango;
        }
        // Juntamos los hilos
        for(auto& h: vhilos)
            h.join();

        // Total general
        auto total = accumulate(begin(result), end(result), 0);

        cout << "Total: " << total << endl;
    }

}