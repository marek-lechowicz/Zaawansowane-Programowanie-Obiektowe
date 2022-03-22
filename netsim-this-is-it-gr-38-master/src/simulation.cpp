// Created by mlech on 22.12.2020.
// Created by kacpe on 22.12.2020.
//
#include "simulation.hpp"


void simulate(Factory& f, TimeOffset d, std::function<void(Factory&,Time)> generate_raport) {

    Time t = 1;
    while(t <= d) {
        // ################### DOSTAWA ##################
        f.do_delivers(t);


        // ################### PZEKAZANIE ##################
        f.do_package_passing();

        // ################### PRZETWORZENIE ##################
        f.do_work(t);

        // ################### RAPORTOWANIE ##################
        generate_raport(f, t);

        // Zwiększ czas
        t++;
    }

}