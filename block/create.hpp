#ifndef BLOCK_CREATE_HPP
#define BLOCK_CREATE_HPP

#include "visibility.hpp"


namespace plugboard
{
    extern "C"
    DSOEXPORT Block* create();

    Block* create()
    {
        return new PlugBoardBlock;
    }
} // namespace plugboard

#endif // BLOCK_CREATE_HPP
