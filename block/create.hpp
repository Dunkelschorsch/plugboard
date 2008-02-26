#ifndef BLOCK_CREATE_HPP
#define BLOCK_CREATE_HPP

#include "visibility.hpp"

// #define HumpBlock HumpBlock

namespace
{
    extern "C"
    DSOEXPORT Block* create();

    Block* create()
    {
        return new HumpBlock;
    }
}

#endif // BLOCK_CREATE_HPP
