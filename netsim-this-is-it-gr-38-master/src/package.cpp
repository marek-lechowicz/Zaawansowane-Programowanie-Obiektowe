//
// Created by User on 13.12.2020.
//

#include "../include/package.hpp"
#include "types.hpp"
#include <set>


Package & Package::operator=(Package&& other) {
    if (this != &other) {
        freed_IDs.insert(id_);
        assigned_IDs.erase(id_);
        id_ = std::move(other.id_);
        other.id_ = MOVED;
    }
    return *this;
}

Package::Package(ElementID id) {
    if (assigned_IDs.find(id) == assigned_IDs.end()) {
        id_ = id;
        assigned_IDs.insert(id);
        if (freed_IDs.find(id) != freed_IDs.end()) {
            freed_IDs.erase(id);
        }
    } else {
        throw std::runtime_error("ID already used!");
    }
}
