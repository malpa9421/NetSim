#include "../include/storage_types.hxx"

Package PackageQueue::pop()
{
    auto& list = package_list_;
    Package result;

    bool lifo = (package_queue_type_ == PackageQueueType::LIFO);

    if (lifo)
    {
        result = std::move(list.back());
        list.pop_back();
    }
    else
    {
        result = std::move(list.front());
        list.pop_front();
    }

    return result;
}
