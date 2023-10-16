/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** Audio
*/

#ifndef AUDIO_HPP_
#define AUDIO_HPP_

#include <string>

class Audio {
    public:
        Audio(float volume, const std::string &path, bool loop) {
            _volume = volume;
            std::string _path = path;
            bool _loop = loop;
        };

        float _volume;
        std::string _path;
        bool _loop;
};

#endif /* !AUDIO_HPP_ */
