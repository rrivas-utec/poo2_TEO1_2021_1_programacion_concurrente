//
// Created by rudri on 7/24/2021.
//

#include "ejemplo_mutex.h"

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>

using namespace std;

mutex mtx; // Debe crearse un ambito compartir con los hilos

static void incrementar(long int& x) {
    // lock_guard lk(mtx); // lock
    mtx.lock();
    // ----Sección Critica----
    ++x;            // si ocurre un error se puede generar live lock
    // -----------------------
    mtx.unlock();   // Si no lo pongo lock leak
}

void ejemplo_mutex() {
    // variable
    int nrep = 100;
    int nhil = 100;

    // loop para representa las repeticion
    for (int i =0; i < nrep; ++i) {
        long int x = 1;

        // creando n hilos
        vector<thread> vhilos(nhil);

        // instanciando los hilos
        for(auto& hilo: vhilos)
            hilo = thread(incrementar, ref(x));

        // uniendo los hilos con el hilo principal
        for (auto& hilo: vhilos)
            hilo.join();

        cout << x << " ";
    }
}
