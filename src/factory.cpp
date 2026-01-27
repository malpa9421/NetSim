#include "../include/factory.hxx"
#include "../include/nodes.hxx"
#include <sstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

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
    for (auto& worker : workers) {
        visited[&worker] = NodeColor::UNVISITED;
    }
    // inicjalizacja dla wszystkich ramp
    for (auto& ramp : ramps) {
        visited[&ramp] = NodeColor::UNVISITED;
    }

    try {
        for (auto& ramp : ramps) {
            has_reachable_storehouse(&ramp, visited);
        }
    } catch (const std::logic_error&) {
        return false;
    }

    return true;
}

void Factory::do_deliveries(Time t) {
    for (auto& ramp : ramps) {
        ramp.deliver_goods(t);
    }
}

void Factory::do_work(Time t) {
    for (auto& worker : workers) {
        worker.do_work(t);
    }
}

void Factory::do_package_passing() {
    for (auto& ramp : ramps) {
        ramp.send_package();
    }

    for (auto& worker : workers) {
        worker.send_package();
    }
}



std::istringstream tekst(R"(
; == LOADING RAMPS ==

LOADING_RAMP id=1 delivery-interval=3
LOADING_RAMP id=2 delivery-interval=2

; == WORKERS ==

WORKER id=1 processing-time=2 queue-type=FIFO
WORKER id=2 processing-time=1 queue-type=LIFO

; == STOREHOUSES ==

STOREHOUSE id=1

; == LINKS ==

LINK src=ramp-1 dest=worker-1

LINK src=ramp-2 dest=worker-1
LINK src=ramp-2 dest=worker-2

LINK src=worker-1 dest=worker-1
LINK src=worker-1 dest=worker-2

LINK src=worker-2 dest=store-1
)");




ParsedLineData parse_line(std::string line) {
    ParsedLineData parsedLine;

    std::istringstream token_stream(line);
    char delimiter = ' ';

    std::string tag;
    std::getline(token_stream, tag, delimiter);

    static const std::map<std::string, ElementType> viableTypes = {
        {"LOADING_RAMP", ElementType::RAMP},
        {"WORKER",        ElementType::WORKER},
        {"STOREHOUSE",    ElementType::STOREHOUSE},
        {"LINK",          ElementType::LINK}
    };

    //Czy linia zaczyna się od jednego z predefiniowanych znaczników
    if (viableTypes.find(tag) != viableTypes.end()) {
        parsedLine.elementType = viableTypes.at(tag);
    }
    else {
        throw std::runtime_error("Niepoprawny znacznik" + tag);
    }


    std::string token;
    while (std::getline(token_stream, token, delimiter)) {
        auto pos = token.find('=');
        if (pos != std::string::npos) { // Wiem, że sprawdzanie poprawnosci ma być w oddzielnej fun. ale dałem dla bezpieczeństwa
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos + 1);
            parsedLine.parameters[key] = value;
        } else {
            throw std::runtime_error("Niepoprawny format parametrów" + token);
        }
    }

    return parsedLine;
}


Factory load_factory_structure(std::istream& is) {
    Factory factory;

    std::string line;
    while (std::getline (is, line)) {
        if (line[0] != ';' and !line.empty()) {
            //Bez sensu jest najpierw dodawanie danych do kontenera, a sprawdzenie ich dopiero potem, ale tak nakazuje polecenie
            auto parsedLine = parse_line(line);

            //Analizą poprawności semantycznej
            switch(parsedLine.elementType) {
                case ElementType::RAMP:
                    if (parsedLine.parameters.count("id") == 1 and
                        parsedLine.parameters.count("delivery-interval") == 1) {
                        try {
                            int id = std::stoi(parsedLine.parameters.at("id"));
                            int deliveryInterval = std::stoi(parsedLine.parameters.at("delivery-interval"));

                            Ramp ramp(id, deliveryInterval);
                            factory.add_ramp(std::move(ramp));

                        } catch (const std::exception&) {
                            throw std::runtime_error("Niepoprawne wartości parametrów"); //Możnaby dodać liczenie numeru linii i zwraanie w errorze
                        }
                    }
                else {
                    throw std::runtime_error("Brak wymaganych parametrów");
                }
                    break;

                case ElementType::WORKER:
                    if (parsedLine.parameters.count("id") == 1 and
                    parsedLine.parameters.count("processing-time") == 1 and
                    parsedLine.parameters.count("queue-type") == 1){
                        try {
                            int id = std::stoi(parsedLine.parameters.at("id"));
                            int processingTime = std::stoi(parsedLine.parameters.at("processing-time"));

                            PackageQueueType package_queue_t;
                            const std::string& queue_type_str = parsedLine.parameters.at("queue-type");

                            if (queue_type_str == "LIFO") {
                                package_queue_t = PackageQueueType::LIFO;
                            }
                            else if (queue_type_str == "FIFO") {
                                package_queue_t = PackageQueueType::FIFO;
                            }
                            else {
                                throw std::runtime_error("Niepoprawny typ kolejki: " + queue_type_str);
                            }


                            Worker worker(id, processingTime, std::make_unique<PackageQueue>(package_queue_t));
                            factory.add_worker(std::move(worker));
                        } catch (const std::exception&) {
                            std::cout<<"Niepoprawne wartości parametrów";
                        }

                        }
                    else {
                        throw std::runtime_error("Brak wymaganych parametrów");
                    }
                    break;

                case ElementType::STOREHOUSE:
                    if (parsedLine.parameters.count("id") == 1) {
                        try {
                            int id = std::stoi(parsedLine.parameters.at("id"));

                            Storehouse storehouse(id);
                            factory.add_storehouse(std::move(storehouse));
                        } catch (const std::exception&) {
                            std::cout<<"Niepoprawne wartości parametrów";
                        }
                        }
                    else {
                        throw std::runtime_error("Brak wymaganych parametrów");
                    }
                break;

                case ElementType::LINK:
                    enum class NodeType {
                    RAMP, WORKER, STORE
                    };

                    //Niby złamanie DRY, ale tak jest chyba lepiej niż jedna mapa globalna
                    std::map<std::string, NodeType> str_node_type{
                    {"ramp", NodeType::RAMP},
                    {"worker", NodeType::WORKER},
                    {"store", NodeType::STORE}
                    };

                    std::string src_str = parsedLine.parameters.at("src");
                    std::string dest_str = parsedLine.parameters.at("dest");

                    NodeType src_node_t;
                    ElementID src_node_id;
                    NodeType dest_node_t;
                    ElementID dest_node_id;

                    auto pos = src_str.find('-');
                    if (pos != std::string::npos) {
                        src_node_t = str_node_type.at(src_str.substr(0, pos));
                        src_node_id = std::stoi(src_str.substr(pos + 1));
                    } else {
                        throw std::runtime_error("Niepoprawny format parametrów: " + src_str);
                    }

                    pos = dest_str.find('-');
                    if (pos != std::string::npos) {
                        dest_node_t = str_node_type.at(dest_str.substr(0, pos));
                        dest_node_id = std::stoi(dest_str.substr(pos + 1));
                    } else {
                        throw std::runtime_error("Niepoprawny format parametrów: " + dest_str);
                    }


                    IPackageReceiver* package_receiver = nullptr;

                    switch(dest_node_t) {
                        case NodeType::RAMP:
                            break;
                        case NodeType::WORKER:
                            auto it = factory.find_worker_by_id(dest_node_id);
                            package_receiver = &*factory.find_worker_by_id(dest_node_id);
                        break;
                        case NodeType::STORE: {
                            package_receiver = &*factory.find_storehouse_by_id(dest_node_id);
                            break;
                        }
                    }

                    switch(src_node_t) {
                        case NodeType::RAMP: {
                            factory.find_ramp_by_id(src_node_id)->receiver_preferences_.add_receiver(package_receiver);
                            break;
                        }
                        case NodeType::WORKER: {
                            factory.find_worker_by_id(src_node_id)->receiver_preferences_.add_receiver(package_receiver);
                            break;
                        }
                        case NodeType::STORE:
                            break;
                    }


                    break;
            }


        }

    }

    return factory;
}

enum class NodeType {
    RAMP, WORKER, STORE
    };

static const std::map<NodeType, std::string> type_to_str{
        {NodeType::RAMP, "ramp"},
        {NodeType::WORKER, "worker"},
        {NodeType::STORE, "store"}
};


//PROBLEM Z get_receiver_type() !!!
std::stringstream save_links(PackageSender& sender, ElementID sender_id, std::string sender_type) { //Zapisujemy id bo PackageSender nie ma get_id
    std::stringstream out;
    auto& receivers = sender.receiver_preferences_.get_preferences();
    for (const auto& [receiver_ptr, probability] : receivers) {
        ReceiverType receiver_type = receiver_ptr->get_receiver_type();

        out << "LINK src=" << sender_type << "-" << sender_id << " ";
    }
}


void save_factory_structure(Factory& factory, std::ostream& os) {
    std::string links;

    os << "; == LOADING RAMPS ==\n";
    std::for_each(factory.ramp_cbegin(), factory.ramp_cend(), [&](Ramp& ramp) {
        ElementID ramp_id = ramp.get_id();
        save_links(ramp, ramp_id, "ramp");
        os << "LOADING_RAMP id=" << ramp_id << ' ' << "delivery-interval=" << ramp.get_delivery_interval() << '\n';
    });

    os << "; == WORKERS ==\n";
    std::for_each(factory.worker_cbegin(), factory.worker_cend(), [&](const Worker& worker) {
        auto& queue = *worker.get_queue();
        PackageQueueType queue_type = queue.get_queue_type();

        std::string queue_type_str;
        if (queue_type == PackageQueueType::LIFO) {
            queue_type_str = "LIFO";
        }
        else if (queue_type  == PackageQueueType::FIFO) {
            queue_type_str = "FIFO";
        }

        ElementID worker_id = worker.get_id();
        os << "WORKER id=" << worker_id << ' '
           << "processing-time=" << worker.get_processing_duration() << ' '
           << "queue-type=" << queue_type_str << '\n';
    });

    os << "; == STOREHOUSES ==\n";
    std::for_each(factory.storehouse_cbegin(), factory.storehouse_cend(), [&](const Storehouse& storehouse) {
        os << "STOREHOUSE id=" << storehouse.get_id() << '\n';
    });

    os.flush();
}

