/*
** EPITECH PROJECT, 2023
** R-Type
** File description:
** ZLib
*/

#ifndef ZLIB_HPP_
#define ZLIB_HPP_

#include <cstdint>
#include "zlib.h"

namespace zlib
{
    template<typename T>
    static int compressBuf(T &dest, const T &src)
    {
        return compress(dest, sizeof(dest), src, sizeof(src));
    }

    template<typename T>
    static int decompressBuf(T &dest, const T &src)
    {
        return uncompress(dest, sizeof(dest), src, sizeof(src));
    }
}

#endif /* !ZLIB_HPP_ */
