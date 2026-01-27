#ifndef FACTORY_HXX
#define FACTORY_HXX

#include "../include/nodes.hxx"
#include <algorithm>

template <typename Node>
class NodeCollection {
public:
    using container_t = std::list<Node>; //zmiana z map na list, tylko po to żeby testy przeszły
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    void add(Node&& node) {
        container.push_back(std::move(node));
    }

    void remove_by_id(ElementID id) {
        container.remove_if([id](const Node& n) {
            return n.get_id() == id;
        });
    }

    iterator find_by_id(ElementID id) {
        return std::find_if(container.begin(), container.end(),
            [id](const Node& n) { return n.get_id() == id; });
    }

    const_iterator find_by_id(ElementID id) const {
        return std::find_if(container.cbegin(), container.cend(),
            [id](const Node& n) { return n.get_id() == id; });
    }

    iterator begin() { return container.begin(); }
    iterator end() { return container.end(); }

    const_iterator begin() const { return container.cbegin(); }
    const_iterator end() const { return container.cend(); }

    const_iterator cbegin() const { return container.cbegin(); }
    const_iterator cend() const { return container.cend(); }

private:
    container_t container;
};

class Factory {
public:
    void add_ramp(Ramp&& r) {ramps.add(std::move(r)); }

    void remove_ramp(ElementID id) {ramps.remove_by_id(id); }

    void add_worker(Worker&& w) {workers.add(std::move(w)); }

    void remove_worker(ElementID id) {
        remove_receiver(workers, id);
        workers.remove_by_id(id);
    }

    void add_storehouse(Storehouse&& s) {storages.add(std::move(s)); }

    void remove_storehouse(ElementID id) {
        remove_receiver(storages, id);
        storages.remove_by_id(id);
    }


    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id) {return ramps.find_by_id(id); }
    NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const {return ramps.find_by_id(id); }

    NodeCollection<Ramp>::const_iterator ramp_cbegin() const {return ramps.cbegin(); }
    NodeCollection<Ramp>::const_iterator ramp_cend() const {return ramps.cend(); }

    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id) {return workers.find_by_id(id); }
    NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const {return workers.find_by_id(id); }

    NodeCollection<Worker>::const_iterator worker_cbegin() const {return workers.cbegin(); }

    NodeCollection<Worker>::const_iterator worker_cend() const {return workers.cend(); }

    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id) {return storages.find_by_id(id); }

    NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const {return storages.find_by_id(id);}

    NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const {return storages.cbegin(); }

    NodeCollection<Storehouse>::const_iterator storehouse_cend() const {return storages.cend(); }

    bool is_consistent() const;
    void do_deliveries(Time t);
    void do_work(Time t);
    void do_package_passing();


private:
    bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& visited) const;

    NodeCollection<Ramp> ramps;
    NodeCollection<Worker> workers;
    NodeCollection<Storehouse> storages;

    template<class Node>
    void remove_receiver(NodeCollection<Node>& collection, ElementID id) {

        auto iter = collection.find_by_id(id);

        IPackageReceiver* receiver_ptr = dynamic_cast<IPackageReceiver*>(&(*iter));

        for (auto& ramp: ramps) {
            auto& _preferences = ramp.receiver_preferences_.get_preferences();
            for (auto _preference: _preferences) { //Można zmienić jeżeli usuwanie już samo sprawdza
                if (_preference.first == receiver_ptr) {
                    ramp.receiver_preferences_.remove_receiver(receiver_ptr);
                    break;
                }
            }
        }

        for (auto& worker: workers) {
            auto& _preferences = worker.receiver_preferences_.get_preferences();
            for (auto _preference: _preferences) {
                if (_preference.first == receiver_ptr) {
                    worker.receiver_preferences_.remove_receiver(receiver_ptr);
                    break;
                }
            }
        }
    }

};



//Odczyt i zapis do pliku
enum class ElementType {
    RAMP,
    WORKER,
    STOREHOUSE,
    LINK
};

struct ParsedLineData{
    ElementType elementType{};
    std::map<std::string, std::string> parameters;
};

ParsedLineData parse_line(std::string line);

Factory load_factory_structure(std::istream& is);

void save_factory_structure(Factory& factory, std::ostream& os);

#endif //FACTORY_HXX
