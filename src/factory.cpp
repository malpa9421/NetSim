#include "factory.hxx"
#include "nodes.hxx"

bool Factory::has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& visited) const {
    if (visited[sender] == NodeColor::VERIFIED) {
        return true;
    }
    if (visited[sender] == NodeColor::VISITED) {
        return true;
    }

    visited[sender] = NodeColor::VISITED;

    auto mutable_sender = const_cast<PackageSender*>(sender);
    const auto& prefs = mutable_sender->receiver_preferences_.get_preferences();

    if (prefs.empty()) {
        throw std::logic_error("brak odbiorców dla węzła");
    }

    bool can_reach = false;
    for (auto& [receiver, prob] : prefs) {
        // czy odbiorca to Storehouse
        if (dynamic_cast<Storehouse*>(receiver)) {
            can_reach = true;
        }
        // czy odbiorca to Worker
        else {
            auto worker = dynamic_cast<Worker*>(receiver);
            if (worker) {
                const PackageSender* next_sender = dynamic_cast<const PackageSender*>(worker);
                if (has_reachable_storehouse(next_sender, visited)) {
                    can_reach = true;
                }
            }
        }
        if (can_reach) break;
    }

    if (can_reach) {
        visited[sender] = NodeColor::VERIFIED;
        return true;
    }

    throw std::logic_error("nie odnaleziono drogi do magazynu");
}

bool Factory::is_consistent() const {
    std::map<const PackageSender*, NodeColor> visited;

    // inicjalizacja dla wszystkich pracowników
    for (auto it = workers.begin(); it != workers.end(); ++it) {
        visited[&it->second] = NodeColor::UNVISITED;
    }
    // inicjalizacja dla wszystkich ramp
    for (auto it = ramps.begin(); it != ramps.end(); ++it) {
        visited[&it->second] = NodeColor::UNVISITED;
    }

    try {
        for (auto it = ramps.begin(); it != ramps.end(); ++it) {
            has_reachable_storehouse(&it->second, visited);
        }
    } catch (const std::logic_error&) {
        return false;
    }

    return true;
}

void Factory::do_deliveries(Time t) {
    for (auto it = ramps.begin(); it != ramps.end(); ++it) {
        it->second.deliver_goods(t);
    }
}

void Factory::do_work(Time t) {
    for (auto it = workers.begin(); it != workers.end(); ++it) {
        it->second.do_work(t);
    }
}

void Factory::do_package_passing() {
    for (auto it = ramps.begin(); it != ramps.end(); ++it) {
        it->second.send_package();
    }

    for (auto it = workers.begin(); it != workers.end(); ++it) {
        it->second.send_package();
    }
}
