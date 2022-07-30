#include <../headers/object.hpp>
#include <sys/stat.h>
#include <sys/types.h>

const std::string Object::getPath()
{
    std::string pathDir = "./.gitAtHome/objects/" + hash.substr(0, 2);

    struct stat st = {0};

    if (stat(pathDir.c_str(), &st) == -1) { // check if directory exists
        mkdir(pathDir.c_str(), 0777); // testing purposes
    }

    return "./.gitAtHome/objects/" + hash.substr(0, 2) + "/" + hash.substr(2);
}