//
// Created by pollo on 6/8/2022.
//

#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <string>
#include <cstdlib>

using json = nlohmann::json;

class FileSystem
{
public:
    static std::string getPath(const std::string& path);
    json readJsonFromFile(const std::string& filename);
    void getAllKeys(const json& jsonObject, std::vector<std::string>& keys);

private:
    typedef std::string (*Builder) (const std::string& path);

    static std::string const & getRoot();

    static Builder getPathBuilder();

    static std::string getPathRelativeRoot(const std::string& path);

    static std::string getPathRelativeBinary(const std::string& path);

};

#endif //FILESYSTEM_H
