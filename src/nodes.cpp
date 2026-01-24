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

void Worker::receive_package(Package&& p) {
    q_->push(std::move(p));
}

void Worker::do_work(Time t) {
    if (!processing_buffer_.has_value() && !q_->empty()) {
        processing_buffer_.emplace(q_->pop());
        t_ = t;
    }

    if (processing_buffer_.has_value()) {
        if (t - t_ + 1 >= pd_) {
            push_package(std::move(processing_buffer_.value()));

            processing_buffer_.reset();
        }
    }
}

void Ramp::deliver_goods(Time t) {
    if (t == 1 || (t - t_) >= t_offset_) {
        push_package(Package());
        t_ = t;
    }
}