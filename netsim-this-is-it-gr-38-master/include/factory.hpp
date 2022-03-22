//
// Created by kacpe on 13.12.2020.
//

#ifndef NET_SIMULATION_FACTORY_HPP
#define NET_SIMULATION_FACTORY_HPP

#include "nodes.hpp"
#include <vector>
#include <sstream>


template <class Node>
class NodeCollection {
public:
    using list = typename std::list<Node>; //mozna zmienic na std::vector, nwm co lepsze w sumie
    using iterator = typename list::iterator;
    using const_iterator = typename list::const_iterator;

    iterator begin() {return list_.begin();}
    iterator end() {return list_.end();}
    const_iterator cbegin() const {return list_.cbegin();}
    const_iterator cend() const {return list_.cend();}
    const_iterator begin() const {return list_.cbegin();}
    const_iterator end() const {return list_.cend();}

    void add(Node&& node);
    void remove_by_id(ElementID id_); //TODO: ZAIMPLEMENTOWAC POZA KLASA
    iterator find_by_id(ElementID id_) {
        return std::find_if(list_.begin(), list_.end(), [id_](const auto& elem) {return id_==elem.get_id();}); //FIXME Czy na pewno nie trzeba lambdy?
    }
    const_iterator find_by_id(ElementID id_) const {
        return std::find_if(list_.cbegin(), list_.cend(), [id_](const auto& elem) {return id_==elem.get_id();});
    }
private:
    list list_;
};

class Factory{
public:
    // RAMP
    void add_ramp(Ramp&& ramp) { ramps_.add(std::move(ramp)); }
    void remove_ramp(ElementID id) { ramps_.remove_by_id(id); }

    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id) {return ramps_.find_by_id(id);}
    NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const { return ramps_.find_by_id(id);}
    NodeCollection<Ramp>::const_iterator ramp_cbegin() const {return ramps_.cbegin();}
    NodeCollection<Ramp>::const_iterator ramp_cend() const {return ramps_.cend();}

    //WORKER
    void add_worker(Worker&& worker) { workers_.add(std::move(worker)); }
    void remove_worker(ElementID id) {
        remove_receiver(workers_, id);
        workers_.remove_by_id(id);
    }

    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id) {return workers_.find_by_id(id);}
    NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const {return workers_.find_by_id(id);}
    NodeCollection<Worker>::const_iterator worker_cbegin() const {return workers_.cbegin();}
    NodeCollection<Worker>::const_iterator worker_cend() const {return workers_.cend();}

    //STOREHOUSE
    void add_storehouse(Storehouse&& storehouse) { storehouses_.add(std::move(storehouse)); }
    void remove_storehouse(ElementID id) {
        remove_receiver(storehouses_, id);
        storehouses_.remove_by_id(id);
    }

    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id) {return storehouses_.find_by_id(id);}
    NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const {return storehouses_.find_by_id(id);}
    NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const {return storehouses_.cbegin();}
    NodeCollection<Storehouse>::const_iterator storehouse_cend() const {return storehouses_.cend();}

    enum class NodeColor { UNVISITED, VISITED, VERIFIED };
    bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors) const;
    bool is_consistent() const;
    void do_delivers(Time t);
    void do_package_passing();
    void do_work(Time t);

private:
    NodeCollection<Ramp> ramps_;
    NodeCollection<Worker> workers_;
    NodeCollection<Storehouse> storehouses_;

    template <typename Node>
    void remove_receiver(NodeCollection<Node>& collection, ElementID id);
};



template <typename Node>
void Factory::remove_receiver(NodeCollection<Node>& collection, ElementID id) {
    auto to_erase = &(*collection.find_by_id(id));

    for (auto& worker: workers_) {
        for (auto& receiver : worker.receiver_preferences_.get_preferences()){
            if (receiver.first == to_erase) {
                worker.receiver_preferences_.remove_receiver(receiver.first);
                break;
            }
        }
    }

    for (auto& ramp: ramps_) {
        for (auto& receiver : ramp.receiver_preferences_.get_preferences()) {
            if (receiver.first == to_erase) {
                ramp.receiver_preferences_.remove_receiver(receiver.first);
                break;
            }
        }
    }
}

template<class Node>
void NodeCollection<Node>::add(Node&& node) {
    list_.push_back(std::move(node));
}

template<class Node>
void NodeCollection<Node>::remove_by_id(ElementID id_) {
    if (find_by_id(id_) != list_.end()) { //FIXME NOWE
        list_.erase(find_by_id(id_));
    }
}

enum class ElementType{
    RAMP, WORKER, STOREHOUSE, LINK
};

struct ParsedLineData {
    ParsedLineData(ElementType el_type, std::map<std::string, std::string> map): element_type(el_type),  parameters (std::move(map)) {};

    ElementType element_type;
    std::map<std::string, std::string> parameters;
};


ParsedLineData parse_line (std::string line);
Factory load_factory_structure(std::istream& is);
void save_factory_structure (Factory& factory, std::ostream& os);



#endif //NET_SIMULATION_FACTORY_HPP
