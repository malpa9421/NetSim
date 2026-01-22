#include "../include/package.hxx"

std::set<ElementID> Package::used_ids = {};
std::set<ElementID> Package::freed_ids = {};

Package::Package() {
    // ustawiamy id na pierwszy element ze zwolnionych jesli zbior niepusty
    if (!freed_ids.empty()) {
        id_ = *freed_ids.begin();
        freed_ids.erase(freed_ids.begin());
    } else {
        // ^ jesli nie ma uzywanych id_ to 1; jesli sa, to (ostatni_z_listy+1)
        id_ = used_ids.empty() ? 1 : (*used_ids.rbegin() + 1);
    }
    used_ids.insert(id_);
}

// konstruktor z dobranym przez nas id
Package::Package(ElementID id) : id_(id) {
    used_ids.insert(id_);
}

// przekazywanie id innemu
Package::Package(Package&& other) noexcept : id_(other.id_) {
    other.id_ = 0;
}

Package &Package::operator=(Package &&other) noexcept {
    // sprawdzenie przypisania do samego siebie
    if (this == &other) return *this;

    // jesli obiekt ma id -> usuwamy je
    if (id_ != 0) {
        used_ids.erase(id_);
        freed_ids.insert(id_);
    }

    // kradziez id od drugiego obiektu
    id_ = other.id_;
    // uniewaznienie oryginalu
    other.id_ = 0;

    // zwrocenie samego siebie
    return *this;
}

Package::~Package() {
    if (id_ != 0) {
        used_ids.erase(id_);
        freed_ids.insert(id_);
    }
}