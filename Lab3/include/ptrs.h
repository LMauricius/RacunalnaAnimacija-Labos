#include <memory>

template<class T>
using unique = std::unique_ptr<T>;

template<class T>
using shared = std::shared_ptr<T>;

using std::make_unique;
using std::make_shared;