#include "nodes.hxx"

void ReceiverPreferences::add_receiver(IPackageReceiver *r) {
    auto num_of_receivers = double(preferences_.size());
    if (num_of_receivers == 0)
        preferences_[r] = 1.0;
    else {
        for (auto &rec : preferences_)
            rec.second = 1 / (num_of_receivers + 1);
        preferences_[r] = 1 / (num_of_receivers + 1);
    }
}

void ReceiverPreferences::remove_receiver(IPackageReceiver *r) {
    auto num_of_receivers = double(preferences_.size());
    if (num_of_receivers > 1)
        for (auto &rec : preferences_)
            if (rec.first != r)
                rec.second = 1 / (num_of_receivers - 1);
    preferences_.erase(r);
}

IPackageReceiver *ReceiverPreferences::choose_receiver() {
    auto prob = pg_();
    if (prob >= 0 && prob <= 1) {
        auto rec_prob = 0.0;
        for (auto &rec : preferences_) {
            rec_prob += rec.second;
            if (prob <= rec_prob)
                return rec.first;
        }
    }
    return nullptr;
}

void PackageSender::send_package() {
    IPackageReceiver *receiver;
    if (sending_buffer_.has_value()) {
        receiver = receiver_preferences_.choose_receiver();
        if (receiver != nullptr) {
            receiver->receive_package(std::move(sending_buffer_.value()));
            sending_buffer_.reset();
        }
    }
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