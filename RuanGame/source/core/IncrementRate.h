#pragma once

inline bool IncrementRate(int& _counter, const int _rate) {
    // Tell the caller whether or not they should do something
    // Based on the desired rate
    _counter = (_counter + 1) % _rate;

    if (_counter == 0) {
        return true;
    }

    return false;
}