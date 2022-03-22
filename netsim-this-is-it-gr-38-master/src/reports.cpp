// Created by kacpe on 22.12.2020.
// Created by mlech on 22.12.2020.


#include "reports.hpp"

void generate_structure_report(const Factory& factory, std::ostream& os) {
    os.flush(); //Czyszcze bufor dla pewności

    os << "\n== LOADING RAMPS ==\n";

    std::list<int> ramp_ids; // SORTOWANIE RAMP, TWORZĘ PUSTĄ LISTĘ ID RAMP
    std::for_each(factory.ramp_cbegin(),factory.ramp_cend(),[&ramp_ids](auto& ramp){ramp_ids.push_back(ramp.get_id());}); //biorę listę wszystkich id ramp
    ramp_ids.sort(); //sortuje te id.
    std::list<const Ramp*> ramp_list; //tworzę pustą listę ramp
    for(auto id : ramp_ids) {
        ramp_list.push_back(&*factory.find_ramp_by_id(id)); //tworze listę posortowanych ramp według id
    }

    for(auto& ramp: ramp_list) {
            os<<"\nLOADING RAMP #"<<ramp->get_id()<<"\n";
            os<<"  Delivery interval: "<<ramp->get_delivery_interval()<<"\n";
            os<<"  Receivers:\n";
            std::list<int> r_receiver_storehouse_id;
            std::list<int> r_receiver_worker_id;
            for (auto &worker_receiver: ramp->receiver_preferences_.get_preferences()) {
                if(worker_receiver.first->get_receiver_type() == ReceiverType::STOREHOUSE){
                    r_receiver_storehouse_id.push_back(worker_receiver.first->get_id());
                }
                if(worker_receiver.first->get_receiver_type() == ReceiverType::WORKER){
                    r_receiver_worker_id.push_back(worker_receiver.first->get_id());
                }
            }
            r_receiver_storehouse_id.sort();
            std::list<const Storehouse*> w_receiver_storehouse;
            for (auto id : r_receiver_storehouse_id){
                w_receiver_storehouse.push_back(&*factory.find_storehouse_by_id(id));
            }
            r_receiver_worker_id.sort();
            std::list<const Worker*> w_receiver_worker;
            for (auto id : r_receiver_worker_id){
                w_receiver_worker.push_back(&*factory.find_worker_by_id(id));
            }
            for (auto& storehouse_receiver: w_receiver_storehouse) {
                os<<"    storehouse #"<<storehouse_receiver->get_id()<<"\n";
            }
            for (auto& worker_receiver: w_receiver_worker) {
                os<<"    worker #"<<worker_receiver->get_id()<<"\n";
            }
    }


    os<< "\n\n== WORKERS ==\n";

    std::list<int> worker_ids; // SORTOWANIE RAMP, TWORZĘ PUSTĄ LISTĘ ID RAMP
    std::for_each(factory.worker_cbegin(),factory.worker_cend(),[&worker_ids](auto& worker){worker_ids.push_back(worker.get_id());}); //biorę listę wszystkich id ramp
    worker_ids.sort(); //sortuje te id.
    std::list<const Worker*> worker_list; //tworzę pustą listę ramp
    for(auto id : worker_ids) {
        worker_list.push_back(&*factory.find_worker_by_id(id)); //tworze listę posortowanych ramp według id
    }

    for(auto& worker: worker_list) {
        os<<"\nWORKER #"<<worker->get_id()<<"\n";
        os<<"  Processing time: "<<worker->get_processing_duration()<<"\n";
        if(worker->get_queue()->get_queue_type() == PackageQueueType::FIFO) {
            os<<"  Queue type: FIFO\n";
        }
        if(worker->get_queue()->get_queue_type() == PackageQueueType::LIFO) {
            os<<"  Queue type: LIFO\n";
        }
        os<<"  Receivers:\n";
        std::list<int> w_receiver_storehouse_id;
        std::list<int> w_receiver_worker_id;
        for (auto &worker_receiver: worker->receiver_preferences_.get_preferences()) {
            if(worker_receiver.first->get_receiver_type() == ReceiverType::STOREHOUSE){
                w_receiver_storehouse_id.push_back(worker_receiver.first->get_id());
            }
            if(worker_receiver.first->get_receiver_type() == ReceiverType::WORKER){
                w_receiver_worker_id.push_back(worker_receiver.first->get_id());
            }
        }
        w_receiver_storehouse_id.sort();
        std::list<const Storehouse*> w_receiver_storehouse;
        for (auto id : w_receiver_storehouse_id){
            w_receiver_storehouse.push_back(&*factory.find_storehouse_by_id(id));
        }
        w_receiver_worker_id.sort();
        std::list<const Worker*> w_receiver_worker;
        for (auto id : w_receiver_worker_id){
            w_receiver_worker.push_back(&*factory.find_worker_by_id(id));
        }
        for (auto& storehouse_receiver: w_receiver_storehouse) {
            os<<"    storehouse #"<<storehouse_receiver->get_id()<<"\n";
        }
        for (auto& worker_receiver: w_receiver_worker) {
            os<<"    worker #"<<worker_receiver->get_id()<<"\n";
        }
    }

    os<<"\n\n== STOREHOUSES ==\n";

    std::list<int> storehouses_ids; // SORTOWANIE RAMP, TWORZĘ PUSTĄ LISTĘ ID RAMP
    std::for_each(factory.storehouse_cbegin(),factory.storehouse_cend(),[&storehouses_ids](auto& storehouse){storehouses_ids.push_back(storehouse.get_id());}); //biorę listę wszystkich id ramp
    storehouses_ids.sort(); //sortuje te id.
    std::list<const Storehouse*> storehouse_list; //tworzę pustą listę ramp
    for(auto id : storehouses_ids) {
        storehouse_list.push_back(&*factory.find_storehouse_by_id(id)); //tworze listę posortowanych ramp według id
    }

    for (auto& storehouse: storehouse_list) {
        os<<"\nSTOREHOUSE #"<<storehouse->get_id()<<"\n";
    }
    os<<"\n";
}


void generate_simulation_turn_report(const Factory& f, std::ostream& os, Time t) {
    os << "=== [ Turn: " << t << " ] ===" << std::endl << std::endl;

    os << "== WORKERS ==" << std::endl << std::endl;


    std::for_each(f.worker_cbegin(), f.worker_cend(),
                  [&os, &t](auto &worker) { // główna pętla lecąca po wszystkich pracownikach
                      os << "WORKER #" << worker.get_id() << std::endl;
                      os << "  PBuffer: ";
                      if (worker.get_processing_buffer().has_value()) {
                          Time pt = t - worker.get_package_processing_start_time() + 1;
                          os << "#" << worker.get_processing_buffer().value().get_id() << " (pt = "
                             << pt << ")";
                      } else {
                          os << "(empty)";
                      }
                      os << std::endl;
                      os << "  Queue: ";
                      if (worker.cbegin() != worker.cend()) {
                          os << "#" << (*worker.cbegin()).get_id();
                          auto start = worker.cbegin();
                          start++;
                          std::for_each(start, worker.cend(), [&os, &t, &worker](
                                  auto &package) { // pętla przechodząca po wszystkich elementach a kolejki poza 1.
                              os << ", #" << package.get_id();
                          });

                      } else {
                          os << "(empty)";
                      }
                      os << std::endl;
                      os << "  SBuffer: ";
                      if (worker.get_sending_buffer().has_value()) {
                          os << "#" << worker.get_sending_buffer().value().get_id();
                      } else {
                          os << "(empty)";
                      }
                      os << std::endl << std::endl;
                  });

    os << std::endl; // przerwa między WORKERS a STOREHOUSES

    os << "== STOREHOUSES ==" << std::endl << std::endl;
    std::for_each(f.storehouse_cbegin(), f.storehouse_cend(),
                  [&os](auto &storehouse) {  // główna pętla lecąca po wszystkich magazynach
                      os << "STOREHOUSE #" << storehouse.get_id() << std::endl;
                      os << "  Stock: ";
                      if (storehouse.cbegin() != storehouse.cend()) {
                          os << "#" << (*storehouse.cbegin()).get_id();
                          auto start = storehouse.cbegin();
                          start++;
                          std::for_each(start, storehouse.cend(), [&os, &storehouse](
                                  auto &package) { // pętla przechodząca po wszystkich elementach a kolejki poza 1.
                              os << ", #" << package.get_id();
                          });
                      } else {
                          os << "(empty)";
                      }
                      os << std::endl << std::endl;
                  });
}




