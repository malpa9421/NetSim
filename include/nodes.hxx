#ifndef NODES_HXX
#define NODES_HXX

#include "package.hxx"
#include "types.hxx"
#include "storage_types.hxx"
#include "helpers.hxx"
#include <map>
#include <optional>

enum class ReceiverType {
    WORKER, STOREHOUSE
};

enum class NodeColor {
    UNVISITED, VISITED, VERIFIED
};

class IPackageReceiver {
public:
    virtual ~IPackageReceiver() = default;

    virtual void receive_package(Package &&p) = 0;

    virtual IPackageStockpile::const_iterator cbegin() const = 0;
    virtual IPackageStockpile::const_iterator cend() const = 0;
    virtual IPackageStockpile::const_iterator begin() const = 0;
    virtual IPackageStockpile::const_iterator end() const = 0;

    virtual ElementID get_id() const = 0;
    virtual ReceiverType get_receiver_type() const = 0;
};

class ReceiverPreferences {
public:
    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;

    const_iterator cbegin() const {return preferences_.cbegin();}
    const_iterator cend() const {return preferences_.cend();}
    const_iterator begin() const {return preferences_.cbegin();}
    const_iterator end() const {return preferences_.cend();}

    // explicit ReceiverPreferences(ProbabilityGenerator pg);
    explicit ReceiverPreferences(ProbabilityGenerator pg = probability_generator);
    void add_receiver(IPackageReceiver *r);
    void remove_receiver(IPackageReceiver *r);
    IPackageReceiver *choose_receiver();
    preferences_t &get_preferences() {return preferences_;};

    preferences_t preferences_;
};

class PackageSender {
public:
    PackageSender() = default;
    PackageSender(PackageSender&& movable) = default;
    void send_package();
    std::optional<Package> &get_sending_buffer() {return sending_buffer_;}
    ReceiverPreferences receiver_preferences_;
protected:
    void push_package(Package&&);
private:
    std::optional<Package> sending_buffer_ = std::nullopt;
};

class Ramp : public PackageSender {
    ElementID id_;
    TimeOffset t_offset_;
    Time t_;

public:
    Ramp(ElementID id, TimeOffset  t_offset) : PackageSender(), id_(id),  t_offset_(t_offset) {}
    ElementID get_id() const { return id_; };
    TimeOffset get_delivery_interval() const { return t_offset_; };
    void deliver_goods(Time t);
};

class Storehouse : public IPackageReceiver {
public:
    Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d = std::make_unique<PackageQueue>(PackageQueueType::FIFO)) : id_(id), d_(std::move( d)) {}

    void receive_package(Package&& p) override;

    ElementID get_id() const override { return id_; }
    ReceiverType get_receiver_type() const override { return ReceiverType::STOREHOUSE; };

    IPackageStockpile::const_iterator cbegin() const override { return d_->cbegin(); }
    IPackageStockpile::const_iterator cend() const override { return d_->cend(); }
    IPackageStockpile::const_iterator begin() const override { return d_->begin(); }
    IPackageStockpile::const_iterator end() const override { return d_->end(); }

    IPackageStockpile* get_queue() const { return d_.get(); }

private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> d_;
};

class Worker : public IPackageReceiver, public PackageSender {
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q) : PackageSender(), id_(id), pd_(pd), q_(std::move(q)) {}

    void do_work(Time t);

    TimeOffset get_processing_duration() const { return pd_; }
    Time get_package_processing_start_time() const { return t_; }

    void receive_package(Package&& p) override;

    ElementID get_id() const override { return id_; }
    ReceiverType get_receiver_type() const override { return ReceiverType::WORKER; };

    const std::optional<Package>& get_processing_buffer() const { return processing_buffer_; }

    IPackageStockpile::const_iterator cbegin() const override { return q_->cbegin(); }
    IPackageStockpile::const_iterator cend() const override { return q_->cend(); }
    IPackageStockpile::const_iterator begin() const override { return q_->begin(); }
    IPackageStockpile::const_iterator end() const override { return q_->end(); }

    IPackageQueue* get_queue() const { return q_.get(); }

private:
    ElementID id_;
    TimeOffset pd_;
    Time t_;
    std::unique_ptr<IPackageQueue> q_;
    std::optional<Package> processing_buffer_ = std::nullopt;
};
#endif //NODES_HXX