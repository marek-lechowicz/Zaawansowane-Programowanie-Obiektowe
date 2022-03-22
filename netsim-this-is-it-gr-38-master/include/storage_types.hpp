//
// Created by User on 13.12.2020.
//

#ifndef NET_SIMULATION_STORAGE_TYPES_HPP
#define NET_SIMULATION_STORAGE_TYPES_HPP

#include "package.hpp"
#include "types.hpp"
#include <cstdlib>
#include <stdlib.h>  // TODO: CZEMU?
#include <set>
#include <list>



// klasy IPackageStockpile, IPackageQueue, PackageQueue
// typ wyliczeniowy PackageQueueType

enum class PackageQueueType {
    FIFO,LIFO
};


class IPackageStockpile {
public:
    virtual void push(Package&& package) = 0;
    virtual bool empty() const = 0;
    virtual std::size_t size() const = 0;
    virtual ~IPackageStockpile() = default;


    using const_iterator = std::list<Package>::const_iterator;


    virtual const_iterator begin() const = 0;
    virtual const_iterator cbegin() const = 0;

    virtual const_iterator end() const = 0;
    virtual const_iterator cend() const = 0;

};

class IPackageQueue: public IPackageStockpile {
public:
    virtual Package pop() = 0;
    virtual PackageQueueType get_queue_type() const = 0;
private:

};



class PackageQueue: public IPackageQueue {
public:
    PackageQueue(PackageQueueType type): type_(type) {}
    bool empty() const override { return list_of_packages_.empty(); }
    std::size_t size() const override { return list_of_packages_.size(); }
    void push(Package&& package) override { list_of_packages_.push_back(std::move(package)); }
    PackageQueueType get_queue_type() const override {return type_;}

    Package pop() override;

    IPackageStockpile::const_iterator begin() const override { return list_of_packages_.begin(); }
    IPackageStockpile::const_iterator cbegin() const override { return list_of_packages_.cbegin(); }

    IPackageStockpile::const_iterator end() const override { return list_of_packages_.end(); }
    IPackageStockpile::const_iterator cend() const override { return list_of_packages_.cend(); }

private:
    std::list<Package> list_of_packages_ = {};
    PackageQueueType type_;
};



#endif //NET_SIMULATION_STORAGE_TYPES_HPP
