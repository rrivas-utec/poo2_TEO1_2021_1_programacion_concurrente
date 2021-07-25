cmake_minimum_required(VERSION 3.19)
project(poo2_TEO1_2021_1_programacion_concurrente)

set(CMAKE_CXX_STANDARD 17)

add_executable(poo2_TEO1_2021_1_programacion_concurrente
        main.cpp
        fundamentos.h fundamentos.cpp
        data_race.h data_race.cpp

        ejemplo_mutex.cpp ejemplo_mutex.h modelo_paralelo.cpp modelo_paralelo.h)