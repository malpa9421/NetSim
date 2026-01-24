#ifndef FACTORY_HXX
#define FACTORY_HXX

#include "../include/nodes.hxx"

template <typename Node>
class NodeCollection {
public:
    using container_t = typename std::map<ElementID, Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    void add(Node&& node){container.insert({node.get_id(), std::move(node)});}
    void remove_by_id(ElementID id){container.erase(id);}
    iterator find_by_id(ElementID id ){return container.find(id);}
    const_iterator find_by_id(ElementID id) const {return container.find(id);}

private:
    container_t container;

};

class Factory {
public:
    void add_ramp(Ramp&& r) {ramps.add(std::move(r)); }

    void remove_ramp(ElementID id) {ramps.remove_by_id(id); }

    void add_worker(Worker&& w) {workers.add(std::move(w)); }

    void remove_worker(ElementID id);

    void add_storehouse(Storehouse&& s) {storages.add(std::move(s)); }

    void remove_storehouse(ElementID id);

private:

    NodeCollection<Ramp> ramps;
    NodeCollection<Worker> workers;
    NodeCollection<Storehouse> storages;

    template<class Node>
    void remove_receiver(NodeCollection<Node>& collection, ElementID id);


#endif //FACTORY_H
