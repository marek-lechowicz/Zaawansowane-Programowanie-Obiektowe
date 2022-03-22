//
// Created by mlech on 20.12.2020.
// Created by kacpe on 20.12.2020.
//
#include "nodes.hpp"
#include "factory.hpp"
#include <iostream>

bool Factory::is_consistent() const {
    std::map<const PackageSender*, NodeColor> map_of_colors; //kolor := mapowanie {PackageSender* -> kolor_węzła_enum}
    for (auto& elem : ramps_) {                              //dla wszystkich ramp i dla wszystkich robotników jako `węzeł`: kolor[węzeł] = NIEODWIEDZONY
        map_of_colors[&elem] = NodeColor::UNVISITED;
    }

    for (auto& elem : workers_) {
        map_of_colors[&elem] = NodeColor::UNVISITED;
    }
    for (auto& ramp : ramps_) { //jeśli poniższa instrukcja rzuci wyjątek, zwróć FAŁSZ:  /* tj. sieć nie jest spójna */
        try {                   //dla każdej rampy `rampa` w kolekcji wszystkich ramp w sieci:
            has_reachable_storehouse(&ramp, map_of_colors);  //czy_nadawca_posiada_osiągalny_magazyn(rampa, kolor)
        } catch (std::logic_error &ex) {
            return false;
        }
    }
    return true; //w przeciwnym razie zwróć PRAWDA  /* tj. sieć jest spójna */
}

bool Factory::has_reachable_storehouse(const PackageSender *sender,
                                       std::map<const PackageSender *, NodeColor> &node_colors) const {

    if (node_colors[sender] == NodeColor::VERIFIED) {
        return true;
    }

    node_colors[sender] = NodeColor::VISITED;

    if (sender->receiver_preferences_.begin() == sender->receiver_preferences_.end()) {
//        std::string msg;
//        msg = "Brak zdefiniowanych odbiorców dla nadawcy o ID: " + std::to_string(sender->get_)
        throw std::logic_error("Error in function Factory::has_reachable_storehouse(...) - sender has no defined receivers");
    }

    bool reachable_storehouse = false;
    for (auto& reciever : sender->receiver_preferences_) {
        if (reciever.first->get_receiver_type() == ReceiverType::STOREHOUSE) {
            reachable_storehouse = true;
        }
        else if (reciever.first->get_receiver_type() == ReceiverType::WORKER) {
            IPackageReceiver* receiver_ptr = reciever.first;
            auto worker_ptr = dynamic_cast<Worker*>(receiver_ptr);
            auto sendrecv_ptr = dynamic_cast<PackageSender*>(worker_ptr);
            if (sendrecv_ptr == sender) {
                continue;
            }
            if (node_colors[sendrecv_ptr] == NodeColor::UNVISITED) {
                reachable_storehouse = has_reachable_storehouse(sendrecv_ptr, node_colors);
            }
        }
    }
    node_colors[sender] = NodeColor::VERIFIED;
    if (reachable_storehouse) {
        return true;
    } else {
        throw std::logic_error("Error in function Factory::has_reachable_storehouse(...) - storehouse is unreachable");
    }
}


void Factory::do_delivers(Time t) {
    for (auto& elem : ramps_){
        elem.deliver_goods(t);
    }
}

void Factory::do_package_passing() {
    for (auto& elem : ramps_) {
        elem.send_package();
    }
    for (auto& elem : workers_) {
        elem.send_package();
    }
}

void Factory::do_work(Time t) {
    for (auto& elem : workers_) {
        elem.do_work(t);
    }
}

Factory load_factory_structure(std::istream& is) {
    Factory factory; //Tworzę pusty obiekt factory
    std::string line; //Tworzę obiekt reprezentujący linijkę tekstu
    while (std::getline(is,line)) { //dopóki są jakieś linie
        bool is_blank_line = line.empty() || isblank(line[0]);
        bool is_comment_line = !line.empty() && line[0] == ';';
        if(is_blank_line || is_comment_line) { //gdy pusta linia lub komentarz
            continue;
        }
        ParsedLineData pld = parse_line(line); //pld - parsed_line_data "7" -> int(7)
        if (pld.element_type == ElementType::RAMP) {
            int id = std::stoi(pld.parameters["id"]);
            int delivery_interval = std::stoi(pld.parameters["delivery-interval"]);
            factory.add_ramp(Ramp(id,delivery_interval));

        } if (pld.element_type == ElementType::WORKER) {
            int id = std::stoi(pld.parameters["id"]);
            int processing_time = std::stoi(pld.parameters["processing-time"]);
            PackageQueueType pqt;
            if (pld.parameters["queue-type"] == "FIFO") {
                pqt = PackageQueueType::FIFO;
            } if (pld.parameters["queue-type"] == "LIFO") {
                pqt = PackageQueueType::LIFO;
            }
            factory.add_worker(Worker(id,processing_time,std::make_unique<PackageQueue>(pqt)));
        } if (pld.element_type == ElementType::STOREHOUSE) {
            int id = std::stoi(pld.parameters["id"]);
            factory.add_storehouse(id);
        } if (pld.element_type == ElementType::LINK) {
            std::string::size_type pos_src = pld.parameters["src"].find('-');
            std::string source_type = pld.parameters["src"].substr(0,pos_src); //początek
            int source_id = std::stoi(pld.parameters["src"].substr(pos_src+1)); //do końca

            std::string::size_type pos_dest = pld.parameters["dest"].find('-');
            std::string destiny_type = pld.parameters["dest"].substr(0,pos_dest); //początek
            int destiny_id = std::stoi(pld.parameters["dest"].substr(pos_dest+1)); //do końca

            if (source_type == "ramp") {
                if (destiny_type == "worker") {
                    (factory.find_ramp_by_id(source_id))->receiver_preferences_.add_receiver(&(*(factory.find_worker_by_id(destiny_id)))); //gwiazdka na początku
                }
                if (destiny_type == "store") {
                    (factory.find_ramp_by_id(source_id))->receiver_preferences_.add_receiver(&(*(factory.find_storehouse_by_id(destiny_id))));
                }
            }
            if (source_type == "worker") {
                if (destiny_type == "worker") {
                    factory.find_worker_by_id(source_id)->receiver_preferences_.add_receiver(&(*(factory.find_worker_by_id(destiny_id))));
                }
                if (destiny_type == "store") {
                    factory.find_worker_by_id(source_id)->receiver_preferences_.add_receiver(&(*(factory.find_storehouse_by_id(destiny_id))));
                }
            }
        }
    }
    return factory;
}

ParsedLineData parse_line(std::string line) {
    std::vector<std::string> tokens {}; //Tworzę wektor do którego będę wkładał tokeny
    std::string token; // Tworzę obiekt przechowujący jeden token
    std::istringstream token_stream(line); //Tworzę stream tokenów
    char delimiter = ' '; //Określam według czego dzielić
    while(std::getline(token_stream, token, delimiter)) {
        tokens.push_back(token); // Otrzymuje wektor tokenów
    }

    std::map<std::string,std::string> map_to_return = {}; //inicjalizuje mape
    for (std::size_t i = 1; i<tokens.size();i++) {
        std::string::size_type pos = tokens[i].find('='); //Znajduje gdzie jest =
        map_to_return[tokens[i].substr(0,pos)] = tokens[i].substr(pos+1); // tworzę mapę id - id value itd.
    }
    ElementType el_type; //Sprawdzam wartosc tokens[0] czyli czy to jest loading ramp czy co
    if (tokens[0] == "LOADING_RAMP") {
        el_type = ElementType::RAMP;
    } else if (tokens[0] == "WORKER") {
        el_type = ElementType::WORKER;
    } else if (tokens[0] == "STOREHOUSE") {
        el_type = ElementType::STOREHOUSE;
    } else if (tokens[0] == "LINK") {
        el_type = ElementType::LINK;
    } else {
        throw std::logic_error("Wrong first word!");
    }
    return ParsedLineData(el_type,map_to_return); // Zwracam obiekt typu parsed line
}

void save_factory_structure (Factory& factory, std::ostream& os) {
    os.flush();

    os << "; == LOADING RAMPS ==\n\n";
    std::for_each(factory.ramp_cbegin(),factory.ramp_cend(), [&os](auto& ramp){ os<<"LOADING_RAMP "
                                                                                   "id="<<ramp.get_id() <<" delivery-interval="<<ramp.get_delivery_interval()<<"\n";});

    os <<"\n; == WORKERS ==\n\n";
    std::for_each(factory.worker_cbegin(),factory.worker_cend(), [&os](auto& worker){ os<<"WORKER "
                                                                                    "id="<<worker.get_id() <<" processing-time="<<worker.get_processing_duration()<<
                                                                                    " queue-type=";
                                                                                    if (worker.get_queue()->get_queue_type() == PackageQueueType::FIFO){
                                                                                        os<<"FIFO\n";} if (worker.get_queue()->get_queue_type() == PackageQueueType::LIFO) {
                                                                                        os<<"LIFO\n";
                                                                                    }});

    os <<"\n; == STOREHOUSES ==\n\n";
    std::for_each(factory.storehouse_cbegin(), factory.storehouse_cend(), [&os](auto& storehouse) {os<<"STOREHOUSE id="<<storehouse.get_id()<<"\n";});

    os <<"\n; == LINKS ==\n\n";



    std::for_each(factory.ramp_cbegin(), factory.ramp_cend(), [&os](auto& ramp) {
        std::for_each(ramp.receiver_preferences_.cbegin(), ramp.receiver_preferences_.cend(), [&os, &ramp](auto& item){os << "LINK src=ramp-" << ramp.get_id() << " dest=";
            if(item.first->get_receiver_type() == ReceiverType::STOREHOUSE){
                os<<"store-"<<item.first->get_id()<<"\n";
            }
            if(item.first->get_receiver_type() == ReceiverType::WORKER){
                os<<"worker-"<<item.first->get_id()<<"\n";
            }
        });
    });


    std::for_each(factory.worker_cbegin(), factory.worker_cend(), [&os](auto& worker) {
        std::for_each(worker.receiver_preferences_.cbegin(), worker.receiver_preferences_.cend(), [&os, &worker](auto& item){os << "LINK src=worker-" << worker.get_id() << " dest=";
            if(item.first->get_receiver_type() == ReceiverType::STOREHOUSE){
                os<<"store-"<<item.first->get_id()<<"\n";
            }
            if(item.first->get_receiver_type() == ReceiverType::WORKER){
                os<<"worker-"<<item.first->get_id()<<"\n";
            }
        });
    });
}




