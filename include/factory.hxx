#ifndef FACTORY_HXX
#define FACTORY_HXX

#include "nodes.hxx"

template <typename Node>
class NodeCollection {
public:
    using container_t = typename std::map<ElementID, Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    void add(Node&& node){container.insert({node.get_id(), std::move(node)});}
    void remove_by_id(ElementID id){container.erase(id);}
    iterator find_by_id(ElementID id ){return container.find(id);} //W przypadku neiznalezienia zwraca .end()
    const_iterator find_by_id(ElementID id) const {return container.find(id);}

    iterator begin() {return container.begin(); }
    iterator end() {return container.end(); }

    const_iterator begin() const {return container.cbegin(); }
    const_iterator end() const {return container.cend(); }

    const_iterator cbegin() const {return container.cbegin(); }
    const_iterator cend() const {return container.cend(); }

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


private:

    NodeCollection<Ramp> ramps;
    NodeCollection<Worker> workers;
    NodeCollection<Storehouse> storages;

    template<class Node>
    void remove_receiver(NodeCollection<Node>& collection, ElementID id) {

        auto iter = collection.find_by_id(id);

        IPackageReceiver* receiver_ptr = dynamic_cast<IPackageReceiver*>(&iter->second);

        for (auto& pair: ramps) {
            auto& ramp = pair.second;
            auto& _preferences = ramp.receiver_preferences_.get_preferences();
            for (auto _preference: _preferences) { //Można zmienić jeżeli usuwanie już samo sprawdza
                if (_preference.first == receiver_ptr) {
                    ramp.receiver_preferences_.remove_receiver(receiver_ptr);
                    break;
                }
            }
        }

        for (auto& pair: workers) {
            auto& worker = pair.second;
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

#endif //FACTORY_HXX
