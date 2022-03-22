//
// Created by kacpe on 13.12.2020.
//

#ifndef NET_SIMULATION_NODES_HPP
#define NET_SIMULATION_NODES_HPP
#include <cstdlib>
//#include <stdlib.h>
#include <map>
#include <memory>
//#include <utility>
#include "package.hpp"
#include "storage_types.hpp"
#include "types.hpp"
#include "helpers.hpp"
// klasa IPackageReceiver, Storehouse, ReceiverPreferences, PackageSender, Ramp, Worker
// typ wyliczeniowy ReceiverType

enum class ReceiverType {
    WORKER, STOREHOUSE
};

class IPackageReceiver {
public:
    virtual void receive_package(Package&& p) = 0;
    virtual ElementID get_id() const = 0;
    virtual ReceiverType get_receiver_type() const =0;

    virtual IPackageStockpile::const_iterator begin() const =0;
    virtual IPackageStockpile::const_iterator cbegin() const =0;
    virtual IPackageStockpile::const_iterator end() const =0;
    virtual IPackageStockpile::const_iterator cend() const =0;
};


class ReceiverPreferences {
public:
    ReceiverPreferences() = default;
    ReceiverPreferences(ProbabilityGenerator pg): pg_(pg) {}

    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;

    const_iterator begin() const {return preferences_.begin();}
    const_iterator cbegin() const {return preferences_.cbegin();}

    const_iterator end() const {return preferences_.end();}
    const_iterator cend() const {return preferences_.cend();}

    void add_receiver(IPackageReceiver* r);
    void remove_receiver(IPackageReceiver* r);
    IPackageReceiver* choose_receiver() ;
    const preferences_t& get_preferences() const {return preferences_;}

private:
    preferences_t preferences_;
    ProbabilityGenerator pg_;

};


class PackageSender {
public:
    PackageSender() = default;
    PackageSender(PackageSender&&) = default; //DONE
    void send_package(); //WYSYLANIE W PIZDU
    const std::optional<Package>& get_sending_buffer() const {return buffer_;}

    ReceiverPreferences receiver_preferences_; //MA BYC PUBLIC, DLA UPROSZCZENIA

protected:
    void push_package(Package&&); //WSTAWIANIE DO BUFORA
private:
    std::optional<Package> buffer_;
};


class Ramp: public PackageSender {
public:
    Ramp(ElementID id, TimeOffset di) :
        id_(id), di_(di) {}

    void deliver_goods(Time t);

    TimeOffset get_delivery_interval() const {return di_;}
    ElementID get_id() const {return id_;}
private:
    ElementID id_;
    TimeOffset di_;
};


class Worker: public PackageSender, public IPackageReceiver {
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q) : id_(id), pd_(pd),
                                                                            q_(std::move(q)) {} //DOBRZE CHYBA

    void do_work(Time t);

    TimeOffset get_processing_duration() const { return pd_; }
    IPackageQueue* get_queue() const {return q_.get();}

    Time get_package_processing_start_time() const { return ts_; }
    ReceiverType get_receiver_type() const override {return ReceiverType::WORKER;}
    void receive_package(Package&& p) override { q_->push(std::move(p)); }

    ElementID get_id() const override { return id_; }

    IPackageStockpile::const_iterator begin() const override { return q_->begin(); }
    IPackageStockpile::const_iterator cbegin() const override { return q_->cbegin(); }

    IPackageStockpile::const_iterator end() const override { return q_->end(); }
    IPackageStockpile::const_iterator cend() const override { return q_->cend(); }


    const std::optional<Package>& get_processing_buffer() const { return worker_buffer_; }

private:
    ElementID id_;
    TimeOffset pd_;
    std::unique_ptr<IPackageQueue> q_;
    Time ts_ = 0;
    std::optional<Package> worker_buffer_;
};


class Storehouse: public IPackageReceiver {
public:

    Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d) : id_(id), d_(std::move(d)) {}
    Storehouse(ElementID id): Storehouse(id,std::make_unique<PackageQueue>(PackageQueueType::FIFO)) {} //KONSTR. DELEG.

    ReceiverType get_receiver_type() const override {return ReceiverType::STOREHOUSE;}
    void receive_package(Package&& p) override {d_->push(std::move(p)); }

    ElementID get_id() const override { return id_; }

    IPackageStockpile::const_iterator begin() const override { return d_->begin(); }
    IPackageStockpile::const_iterator cbegin() const override { return d_->cbegin(); }

    IPackageStockpile::const_iterator end() const override { return d_->end(); }
    IPackageStockpile::const_iterator cend() const override { return d_->cend(); }

private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> d_;
};
#endif //NET_SIMULATION_NODES_HPP
