#include "../include/package.hxx"

std::set<ElementID> Package::used_ids = {};
std::set<ElementID> Package::freed_ids = {};

Package::Package() {
    if (!freed_ids.empty()) {
        id_ = *freed_ids.begin();
        freed_ids.erase(freed_ids.begin());
    } else {
        id_ = used_ids.empty() ? 1 : (*used_ids.rbegin() + 1);
    }
    used_ids.insert(id_);
}

Package::Package(ElementID id) : id_(id) {
    used_ids.insert(id_);
}

Package::Package(Package&& other) noexcept : id_(other.id_) {
    other.id_ = 0;
}

Package &Package::operator=(Package &&other) noexcept {
    if (this == &other) return *this;

    if (id_ != 0) {
        used_ids.erase(id_);
        freed_ids.insert(id_);
    }

    id_ = other.id_;
    other.id_ = 0;

    return *this;
}

Package::~Package() {
    if (id_ != 0) {
        used_ids.erase(id_);
        freed_ids.insert(id_);
    }
}