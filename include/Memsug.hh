#pragma once
#include <text.hh>
#include <preftree.hh>

namespace memsug {

class Suger: public std::enable_shared_from_this<Suger> {
public:
    template<typename ...Args>
    auto create(Args&& ...args) -> std::shared_ptr<Suger> {
        return std::shared_ptr<Suger>(new Suger(std::forward<Args>(args)...));
    }

private:
    Suger(text::Dict&& d): dict_{std::move(d)} {}
    text::Dict dict_;
};

}
