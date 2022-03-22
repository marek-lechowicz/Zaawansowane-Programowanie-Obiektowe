//
// Created by User on 13.12.2020.
//

#include "../include/storage_types.hpp"

Package PackageQueue::pop() {
    switch(type_) {
        case PackageQueueType::LIFO: {
            Package package_to_return(std::move(list_of_packages_.back()));
            list_of_packages_.pop_back();
            return package_to_return;
        }
        case PackageQueueType::FIFO: {
            Package package_to_return(std::move(list_of_packages_.front()));
            list_of_packages_.pop_front();
            return package_to_return;
        }
        default: {
            throw std::runtime_error("Queue type doesn't exist.");
        }
    }
}

