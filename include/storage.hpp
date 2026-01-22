#ifndef NETSIM_STORAGE_HPP
#define NETSIM_STORAGE_HPP

class IPackageStockpile {
public:
    using size_type = std::size_t;
    using const_iterator = std::list<Package>::const_iterator;

    virtual ~IPackageStockpile() {}

    virtual void push(Package&& package) = 0;

    virtual bool empty() const = 0;
    virtual size_type size() const = 0;

    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const = 0;
};

#endif //NETSIM_STORAGE_HPP