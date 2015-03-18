#ifndef __NTD_MEMORY_HPP__
#define __NTD_MEMORY_HPP__

#include <memory>
#include <utility>

namespace ntd {

    template<typename T, typename Constructor, typename Destructor, typename... Arguments>
    auto make_smart(Constructor construct, Destructor destruct, Arguments... args) {
        auto ptr = construct(std::forward<Arguments>(args)...);

        if(!ptr) {
            throw std::runtime_error("Err: not able to create smart pointer");
        }

        return std::unique_ptr<T, decltype(destruct)>(ptr, destruct);
    }
}

#endif
