//
// Created by User on 14.12.2020.
//

#include "../include/nodes.hpp"
#include<numeric>

void ReceiverPreferences::add_receiver(IPackageReceiver* r) {
    // OLD IMPLEMENTATION
//    preferences_[r]=probability_generator();
//    double total_sum = std::accumulate(preferences_.begin(),preferences_.end(),0.0
//                    , [] (double acc, const preferences_t::value_type& p)
//                    { return acc + p.second; }); // CHCĘ OTRZYMAC SUME ELEMENTÓW
//    for(auto& item: preferences_) {
//        item.second = item.second/total_sum; // DZIELE KAZDY ELEMENT PRZEZ SUME
//    }
    preferences_[r] = 0; // dodanie odbiorcy do listy
    double number_of_recievers = preferences_.size(); // pobierz aktualną liczbę odbiorców
    for(auto& reciever: preferences_) {
        reciever.second = 1 / number_of_recievers; // zaktualizuj prawdopodobieństwo
    }
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* r) {
    // OLD IMPLEMENTATION
//    preferences_.erase(r);
//    double total_sum = std::accumulate(preferences_.begin(),preferences_.end(),0.0
//            , [] (double acc, const preferences_t::value_type& p)
//                                       { return acc + p.second; }); // CHCĘ OTRZYMAC SUME ELEMENTÓW
//    for(auto& item: preferences_) {
//        item.second = item.second/total_sum; // DZIELE KAZDY ELEMENT PRZEZ SUME
//    }
    preferences_.erase(r); // usuń odbiorcę z listy
    double number_of_recievers = preferences_.size(); // pobierz aktualną liczbę odbiorców
    for(auto& reciever: preferences_) {
        reciever.second = 1 / number_of_recievers; // zaktualizuj prawdopodobieństwo
    }
}

IPackageReceiver* ReceiverPreferences::choose_receiver() {
    double p = probability_generator();
    double distributor_value = 0.0;
    for (auto& item: preferences_) {
        distributor_value = distributor_value + item.second;
        if (p<=distributor_value) {
            return item.first;
        }
    }
    throw std::runtime_error("Error has occurred in ReceiverPreferences::choose_receiver()");
}

void PackageSender::send_package() {
    if(buffer_.has_value()) {
        receiver_preferences_.choose_receiver()->receive_package(std::move(buffer_.value()));
        buffer_.reset();
    }
}


void PackageSender::push_package(Package&& package) {
    if (!buffer_) {
        buffer_.emplace(std::move(package));
    }
}



void Ramp::deliver_goods(Time t) {
    // TODO 02.01.2021
    if (di_ == 1) {
        Package p;
        push_package(std::move(p));
    }
    //
    if (t % di_ == 1) {
        Package p;
        push_package(std::move(p));
    }
}

void Worker::do_work(Time t) {
    if (!worker_buffer_.has_value()) {
        if (!q_->empty()) {
            worker_buffer_.emplace(q_->pop());
            ts_ = t;
        }
    }
    // trzeba sprawdzać czy ma wartość osobnym ifem, bo zależy nam na tym aby
    // w pdrzypadku pd_ = 1, wykonać instrukcje w tej samej turze
    if (worker_buffer_.has_value()) {
        if ((t - ts_) == pd_ - 1) {
            push_package(std::move(worker_buffer_.value()));
            worker_buffer_.reset();
        }
    }
}