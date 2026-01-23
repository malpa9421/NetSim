#ifndef NODES_HXX
#define NODES_HXX

#include "package.hxx"
#include "types.hxx"
#include "storage_types.hxx"
#include "helpers.hxx"
#include <map>
#include <optional>

class IPackageReceiver {
public:
    virtual ~IPackageReceiver() = default;

    virtual void receive_package(Package &&p) = 0;

    virtual IPackageStockpile::const_iterator cbegin() const = 0;
    virtual IPackageStockpile::const_iterator cend() const = 0;
    virtual IPackageStockpile::const_iterator begin() const = 0;
    virtual IPackageStockpile::const_iterator end() const = 0;

    virtual ElementID get_id() = 0;
};

class ReceiverPreferences {
public:
    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;

    const_iterator cbegin() const {return preferences_.cbegin();}
    const_iterator cend() const {return preferences_.cend();}
    const_iterator begin() const {return preferences_.cbegin();}
    const_iterator end() const {return preferences_.cend();}

    explicit ReceiverPreferences(ProbabilityGenerator pg);
    void add_receiver(IPackageReceiver *r);
    void remove_receiver(IPackageReceiver *r);
    IPackageReceiver *choose_receiver();
    preferences_t &get_preferences() {return preferences_;};

    preferences_t preferences_;
};

class PackageSender {
public:
    PackageSender(PackageSender&&) = default;
    void send_package();
    std::optional<Package> &get_sending_buffer();
    ReceiverPreferences receiver_preferences_;
protected:
    void push_package(Package&&);
private:
    std::optional<Package> sending_buffer_ = std::nullopt;
};

#endif //NODES_HXX