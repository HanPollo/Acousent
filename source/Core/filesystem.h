//
// Created by pollo on 6/8/2022.
//

#ifndef BOOMBOOMENGINE_FILESYSTEM_H
#define BOOMBOOMENGINE_FILESYSTEM_H
#include <string>
#include <cstdlib>

class FileSystem
{
public:
    static std::string getPath(const std::string& path);

private:
    typedef std::string (*Builder) (const std::string& path);

    static std::string const & getRoot();

    static Builder getPathBuilder();

    static std::string getPathRelativeRoot(const std::string& path);

    static std::string getPathRelativeBinary(const std::string& path);

};

#endif //BOOMBOOMENGINE_FILESYSTEM_H
