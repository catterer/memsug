#pragma once
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace save {

namespace pt = boost::property_tree;
using blob = pt::ptree;

class dumpable {
public:
    dumpable() = default;
    virtual auto dump() const -> blob = 0;
};

class loadable {
public:
    loadable() = default;
    virtual void load(const blob&) = 0;
};

class serializable:
    public dumpable,
    public loadable
{
public:
    serializable() = default;
};

template<typename T>
T dumpload(T&& val) {
    blob b = val.dump();
    return T(b);
}

}
