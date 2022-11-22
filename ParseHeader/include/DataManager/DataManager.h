#ifndef __DATAMANAGER_H__
#define __DATAMANAGER_H__

#include <string>
#include <set>
#include <map>

class TypeParser;

#ifdef DATAMANAGER_LIBRARY
#define DATAMANAGER_EXPORT __declspec(dllexport)
#else
#define DATAMANAGER_EXPORT __declspec(dllimport)
#endif

class DATAMANAGER_EXPORT DataManager
{
public:
	static DataManager* GetInstance();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	解析文件. </summary>
    ///     1. 获取路径下所有 .h 文件
    ///     2. 解析文件，默认 "../../../src/resources" 文件夹
    /// <remarks>	August295, 2022/9/7. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void FindFiles(const std::string& folder, std::map<std::string, std::map<std::string, bool>>& fileMap);
    void ParseFiles(const std::set<std::string>& includePaths = std::set<std::string>({"../../../src/resources"}));

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	外部数据接口. </summary>
    ///     1.
    /// <remarks>	August295, 2022/9/7. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    std::map<std::string, TypeParser>& GetTypeParserMap();
    std::map<std::string, std::map<std::string, bool>>& GetFileMap();
    void SetFileMap(const std::string& fileName, const std::string& structName, bool change);

private:
    DataManager();
    ~DataManager();

private:
    struct DataManagerPrivate;
    DataManagerPrivate* m_p;
};

#endif
