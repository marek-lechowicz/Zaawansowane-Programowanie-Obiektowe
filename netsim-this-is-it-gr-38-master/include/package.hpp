//
// Created by User on 13.12.2020.
//

#ifndef NET_SIMULATION_PACKAGE_HPP
#define NET_SIMULATION_PACKAGE_HPP
#define MOVED -1


#include <cstdlib>
#include <utility> //zawiera std::move()
#include <algorithm>

#include "types.hpp"
#include <set>


//klasa Package
class Package {
public:
    Package() {
        if (freed_IDs.empty()) {
            if (assigned_IDs.empty()) {
                id_ = 1;
                assigned_IDs.insert(id_);
            } else if (!assigned_IDs.empty()) {
                id_ = *(assigned_IDs.rbegin()) + 1;
                assigned_IDs.insert(id_);
            }
        } else {
            id_ = *(freed_IDs.begin());
            assigned_IDs.insert(id_);
            freed_IDs.erase(freed_IDs.begin());
        }
    }

    explicit Package(ElementID id);
    Package(const Package&) =delete;
    Package(Package&& other): id_(std::move(other.id_)) { other.id_ = MOVED; }

    Package& operator= (Package&& other);
    Package& operator= (const Package& other) = delete;

    ElementID get_id() const {return id_;}
    ~Package() {
        if (id_ != MOVED) {
            freed_IDs.insert(id_);
            assigned_IDs.erase(id_);
        }
    }
private:
    inline static std::set<ElementID> assigned_IDs = {};
    inline static std::set<ElementID> freed_IDs = {};
    ElementID id_;
};

#endif //NET_SIMULATION_PACKAGE_HPP
