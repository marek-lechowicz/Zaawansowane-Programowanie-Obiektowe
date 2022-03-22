//
// Created by kacpe on 13.12.2020.
//

#ifndef NET_SIMULATION_SIMULATION_HPP
#define NET_SIMULATION_SIMULATION_HPP

#include "factory.hpp"

void simulate(Factory& f, TimeOffset d, std::function<void(Factory&,Time)>);


#endif //NET_SIMULATION_SIMULATION_HPP
