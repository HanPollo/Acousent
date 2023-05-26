//
// Created by pollo on 6/8/2022.
//

#ifndef FILESYSTEM_H
#define FILESYSTEM_H
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

#endif //FILESYSTEM_H
