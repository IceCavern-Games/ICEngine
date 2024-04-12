#pragma once

#include <functional>

namespace IC {
    template <class Ret, class... Args> using Func = std::function<Ret(Args...)>;
}
