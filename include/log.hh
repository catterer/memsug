#pragma once
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <mutex>

#include <util.hh>

#define Log(lev) \
    for (int i = 1; i and log::Logger::get().filter() >= log::Level::lev; i = 0) \
        log::Record(log::Level::lev)

#define GET_MACRO(_1,_2,NAME,...) NAME
#define NLog(...) GET_MACRO(__VA_ARGS__, NLog2, NLog1)(__VA_ARGS__)

#define NLog2(l, n) Log(l) << n << ": "
#define NLog1(l) NLog2(l, name())

namespace log {

enum class Level {
    fatal,
    error,
    warn,
    info,
    debug
};

class Logger: public util::Singleton<Logger> {
public:
    Logger() = default;
    void filter(Level l) { filter_ = l; }
    auto filter() const -> Level { return filter_; }

    auto mutex() -> std::mutex& { return mut_; }

private:
    Level filter_{Level::warn};
    std::mutex mut_;
};

std::ostream& operator<<(std::ostream&, Level);

class Record: public std::stringstream {
public:
    using std::stringstream::stringstream;

    Record(Level l): level_{l} {}
    virtual ~Record();

private:
    Level level_{};
};

}
