#include "nodes.hxx"

void ReceiverPreferences::add_receiver(IPackageReceiver *r) {

}

void ReceiverPreferences::remove_receiver(IPackageReceiver *r) {

}

IPackageReceiver ReceiverPreferences::*get_preferences() {
    return nullptr;
}

void PackageSender::send_package() {

}

void PackageSender::push_package(Package &&) {

}
