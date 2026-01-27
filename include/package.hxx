    #ifndef PACKAGE_PACKAGE_HXX
    #define PACKAGE_PACKAGE_HXX

    #include <set>
    #include "types.hxx"

    class Package {
        ElementID id_;

        static std::set<ElementID> used_ids;
        static std::set<ElementID> freed_ids;

    public:
        Package();
        Package(ElementID id);
        Package(Package&& other) noexcept;
        Package& operator=(Package&& other) noexcept;
        ~Package();
        ElementID get_id() const { return id_; }
    };

    #endif