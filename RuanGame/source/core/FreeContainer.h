#pragma once

// use this to actually free up any (standard) container
// (BEWARE this doesn't work on your pools, you already tried it, pls dont do it again)
template<typename T>
inline void FreeContainer(T& p_container)
{
    T empty;
    using std::swap;
    swap(p_container, empty);
}