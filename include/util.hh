#pragma once

namespace util {

class NonCopyable {
public:
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&) = default;
    NonCopyable& operator=(NonCopyable&&) = default;
};

template<typename T>
class Singleton: NonCopyable {
public:
    static auto get() -> T& {
        static T instance_;
        return instance_;
    }

protected:
    Singleton() = default;
    ~Singleton() = default;
};

}
