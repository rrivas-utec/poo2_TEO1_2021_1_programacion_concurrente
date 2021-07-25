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
void acumular(Iterator start, Iterator stop, Iterator result) {
    *result = accumulate(start, stop, 0);
}

void ejemplo_sumarizacion() {
    vector<int> vec_data;
    {
        cout << "Generar vector..." << endl;
        timer_t t;
        vec_data = generar_vector();
        cout << "Size: " << size(vec_data) << endl;
    }
    {
        cout << "Calculo serial..." << endl;
        timer_t t;
        auto total = accumulate(begin(vec_data), end(vec_data), 0); // serial
        cout << "Total: " << total << endl;
    }
    {
        cout << "Calculo paralelo..." << endl;
        timer_t t;
        // Calculo del numero de hilos
        int n_hilos = static_cast<int>(thread::hardware_concurrency()) + 1;
        // Calculo de cantidad de items a procesar por cada hilo
        int rango = ceil(static_cast<double>(size(vec_data)) / (n_hilos));
        // Define el vector de hilos
        vector<thread> vec_hilos (n_hilos);
        // Define el vector de los resultados de hilos
        vector<int> res_hilos (n_hilos);
        // Iterators
        auto it_cur_data = begin(vec_data);     // iterator de vector de datos
        auto it_res_hilos = begin(res_hilos);   // iterator de resultados de hilos
        // Recorrer los hilos
        for (auto& item: vec_hilos) {
            if (distance(it_cur_data, end(vec_data)) < rango)
                rango = distance(it_cur_data, end(vec_data));
            item = thread(
                    [=]{acumular (it_cur_data, next(it_cur_data, rango), it_res_hilos);});
            it_cur_data = next(it_cur_data, rango);
            ++it_res_hilos;
        }
        // Juntamos los hilos
        for(auto& h: vec_hilos)
            h.join();
        // Total general
        auto total = accumulate(begin(res_hilos), end(res_hilos), 0);
        cout << "Total: " << total << endl;
    }
}