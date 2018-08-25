#pragma once

#if __has_include(<optional>)

#   include <optional>
using std::optional;
#elif __has_include(<experimental/optional>)
#   include <experimental/optional>
using std::experimental::optional;
#else
#   error <experimental/optional> and <optional> not found
#endif
