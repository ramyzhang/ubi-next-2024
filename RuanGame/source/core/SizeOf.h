#pragma once

#include <cstddef>

template <class T, std::size_t N>
inline const std::size_t size(const T(&array)[N]) noexcept
{
    return N;
}