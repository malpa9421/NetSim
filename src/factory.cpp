#include "../include/factory.hxx"
#include "../include/nodes.hxx"

template <typename T>
class ClassName {
  public:
    using container_t = typename std::list<Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

private:
  container_t container;

};