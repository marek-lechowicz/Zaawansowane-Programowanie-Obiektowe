//
// Created by kacpe on 13.12.2020.
//

#ifndef NET_SIMULATION_REPORTS_HPP
#define NET_SIMULATION_REPORTS_HPP

#include "factory.hpp"


//klasy SpecificTurnsReportNotifier, IntervalReportNotifier
//funkcje generate_structure_report(), generate_simulation_turn_report()

class SpecificTurnsReportNotifier {
public:
    SpecificTurnsReportNotifier(std::set<Time> turns): turns_(turns) {}
    bool should_generate_report(Time t) {return turns_.find(t) != turns_.end();}
private:
    std::set<Time> turns_;
};

class IntervalReportNotifier {
    IntervalReportNotifier(TimeOffset to) : to_(to) {}

    bool should_generate_report(Time t) { return t % to_ == 1; }

private:
    TimeOffset to_;
};

void generate_structure_report(const Factory& f, std::ostream& os);
void generate_simulation_turn_report(const Factory& f, std::ostream& os, Time t);

#endif //NET_SIMULATION_REPORTS_HPP
