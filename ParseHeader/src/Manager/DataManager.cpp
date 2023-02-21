#include <ParseHeader/TypeParser.h>
#include <ParseHeader/dirent.h>
#include <ParseHeader/utility.h>

#include "DataManager.h"

struct DataManager::DataManagerPrivate {
    std::string                                        m_ResourcesPath; // 资源路径
    std::map<std::string, TypeParser>                  m_TypeParserMap; // 头文件解析 map<文件名, 解析类>
    std::map<std::string, std::map<std::string, bool>> m_FileMap;       // 需解析文件 map<文件名, map<修改结构体, 是否保存>>
};

DataManager::DataManager()
    : m_p(new DataManagerPrivate)
{
    m_p->m_ResourcesPath = "../../resources/";
}

DataManager::~DataManager()
{
}

DataManager* DataManager::GetInstance()
{
    static DataManager m_instance;
    return &m_instance;
}

void DataManager::FindFiles(const std::string& folder, std::map<std::string, std::map<std::string, bool>>& fileMap)
{
    DIR*           dir;
    struct dirent* ent;
    struct stat    entrystat;

    if ((dir = opendir(folder.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string name(ent->d_name);
            if (name.compare(".") == 0 || name.compare("..") == 0)
                continue;

            name = folder + "/" + name;
            if (0 == stat(name.c_str(), &entrystat)) {
                if (S_ISDIR(entrystat.st_mode)) {
                    Info("Searching folder: " + name);
                    FindFiles(name, fileMap);
                } else {
                    if (name.length() > 2 && name.substr(name.length() - 2, 2).compare(".h") == 0) {
                        fileMap.emplace(name, std::map<std::string, bool>());
                        Debug("Found header file: " + name);
                    } else {
                        Info("Ignoring file: " + name);
                    }
                }
            } else {
                Error("failed to stat file/folder: " + name);
            }
        }
        closedir(dir);
    } else {
        Error("failed to open folder: " + folder);
    }
}

void DataManager::ParseFiles(const std::set<std::string>& includePaths)
{
    // 解析指定文件夹下头文件
    std::set<std::string> paths = includePaths;

    for (auto path : includePaths) {
        this->FindFiles(path, m_p->m_FileMap);
        for (auto file : m_p->m_FileMap) {
            std::string filename = file.first;
            TypeParser  parser;
            parser.ParseFile(filename);
            m_p->m_TypeParserMap.emplace(filename, parser);
        }
    }
}

void DataManager::ParseFile(const std::string& fileName)
{
    TypeParser parser;
    parser.ParseFile(fileName);
    m_p->m_TypeParserMap.emplace(fileName, parser);
}

std::string DataManager::GetResourcesPath()
{
    return m_p->m_ResourcesPath;
}

std::map<std::string, TypeParser>& DataManager::GetTypeParserMap()
{
    return m_p->m_TypeParserMap;
}

std::map<std::string, std::map<std::string, bool>>& DataManager::GetFileUpdateMap()
{
    return m_p->m_FileMap;
}

void DataManager::SetFileUpdateMap(const std::string& fileName, const std::string& structName, bool change)
{
    m_p->m_FileMap[fileName].emplace(structName, change);
}

std::list<std::string> DataManager::GetTypeList()
{
    std::list<std::string> typeList;
    // 基础类型
    for (auto basic : data_types) {
        typeList.push_back(basic);
    }
    for (auto parser : m_p->m_TypeParserMap) {
        // 结构体类型
        for (auto structDef : parser.second.struct_defs_) {
            typeList.push_back(structDef.first);
        }
        // 枚举类型
        for (auto enumDef : parser.second.enum_defs_) {
            typeList.push_back(enumDef.first);
        }
    }
    return typeList;
}
