#include "factory.hxx"


bool Factory::is_consistent() const {
    std::map<const PackageSender*, NodeColor> visited;

    for (const auto& w : cont_w) {
        const PackageSender* s = dynamic_cast<const PackageSender*>(&w);
        visited.emplace(s, NodeColor::UNVISITED);
    }

    for (const auto& r : cont_r) {
        const PackageSender* s = dynamic_cast<const PackageSender*>(&r);
        visited.emplace(s, NodeColor::UNVISITED);
    }

    try {
        for (const auto& ramp : cont_r) {
            const PackageSender* sender =
                dynamic_cast<const PackageSender*>(&ramp);

            has_reachable_storehouse(sender, visited);
        }
    }
    catch (std::logic_error&) {
        return false;
    }

    return true;
}

void Factory::do_deliveries(Time t) {
    for (auto it = cont_r.begin(); it != cont_r.end(); ++it) {
        it->deliver_goods(t);
    }
}

void Factory::do_work(Time t) {
    for (auto it = cont_w.begin(); it != cont_w.end(); ++it) {
        it->do_work(t);
    }
}

void Factory::do_package_passing() {
    for (auto it = cont_r.begin(); it != cont_r.end(); ++it) {
        it->send_package();
    }

    for (auto it = cont_w.begin(); it != cont_w.end(); ++it) {
        it->send_package();
    }
}
