//
// Created by pollo on 6/8/2022.
//

#include "filesystem.h"
#include "root_directory.h"

std::string FileSystem::getPath(const std::string &path) {
    static std::string(*pathBuilder)(std::string const &) = getPathBuilder();
    return (*pathBuilder)(path);
}

std::string const &FileSystem::getRoot() {
    static char const * envRoot = getenv("BOOM_BOOM_ROOT_PATH");
    static char const * givenRoot = (envRoot != nullptr ? envRoot : BoomBoom::source_directory);
    static std::string root = (givenRoot != nullptr ? givenRoot : "");
    return root;
}

FileSystem::Builder FileSystem::getPathBuilder() {
    if (getRoot() != "")
        return &FileSystem::getPathRelativeRoot;
    else
        return &FileSystem::getPathRelativeBinary;
}

std::string FileSystem::getPathRelativeRoot(const std::string &path) {
    return getRoot() + std::string("/") + path;
}

std::string FileSystem::getPathRelativeBinary(const std::string &path) {
    return "../../../" + path;
}


