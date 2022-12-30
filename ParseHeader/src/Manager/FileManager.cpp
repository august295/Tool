#include <fstream>

#include "FileManager.h"

FileManager* FileManager::GetInstance()
{
    static FileManager m_instance;
    return &m_instance;
}

FileManager::FileManager()
{
}

FileManager::~FileManager()
{
}

std::vector<std::string> FileManager::GetFileContent(const std::string& file)
{
    std::vector<std::string> fileLineVec;
    std::ifstream            ifs(file.c_str(), std::ios::in);
    if (ifs.fail())
    {
        printf("Failed to open file - %s", file.c_str());
        ifs.close();
        return fileLineVec;
    }
    std::string line;
    while (ifs.good())
    {
        getline(ifs, line);
        fileLineVec.push_back(line);
    }
    ifs.close();

    return fileLineVec;
}

void FileManager::SaveFile(const std::string& file, const std::vector<std::string>& fileLineVec)
{
    std::ofstream ofs(file.c_str(), std::ios::out);
    if (ofs.fail())
    {
        printf("Failed to open file - %s", file.c_str());
        ofs.close();
        return;
    }

    for (auto line : fileLineVec)
    {
        ofs << line << std::endl;
    }
    ofs.close();
}
