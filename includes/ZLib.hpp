/*
** EPITECH PROJECT, 2023
** R-Type
** File description:
** ZLib
*/

#ifndef ZLIB_HPP_
#define ZLIB_HPP_

#include <zlib.h>
#include <vector>
#include <cstdint>

namespace zlib
{
    class ZLib
    {
        public:
            ZLib() = default;
            ~ZLib() = default;

            template<typename T, typename U>
            int compress(T &dest, std::size_t &destSize, const U &src, int compressionLevel)
            {
                const Bytef *data = reinterpret_cast<const Bytef *>(src.data());
                uLong dataLen = static_cast<uLong>(src.size());
                uLong compressedDataSize = compressBound(dataLen);
                dest.resize(compressedDataSize);
                int result = compress2(dest.data(), &compressedDataSize, data, dataLen, compressionLevel);

                if (result == Z_OK) {
                    dest.resize(compressedDataSize);
                    destSize = static_cast<std::size_t>(compressedDataSize);
                }
                return result;
            }

            template<typename T, typename U>
            int decompress(T &dest, const U &src, std::size_t maxSize)
            {
                uLong decompressedSize = static_cast<uLong>(maxSize);
                dest.resize(decompressedSize);
                int result = uncompress(dest.data(), &decompressedSize, src.data(), static_cast<uLong>(src.size()));

                if (result == Z_OK) {
                    dest.resize(decompressedSize);
                } else
                    std::cerr << "Decompression failed with error code: " << result << std::endl;
                return result;
            }

        private:
    };
}

#endif /* !ZLIB_HPP_ */
