#pragma once

template<typename T>
inline void FreeContainer(T& p_container)
{
    T empty;
    using std::swap;
    swap(p_container, empty);
}