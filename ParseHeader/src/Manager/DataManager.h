#ifndef __DATAMANAGER_H__
#define __DATAMANAGER_H__

#include <string>
#include <set>
#include <map>
#include <list>

#include "GlobalManager.h"

class TypeParser;

class MANAGER_EXPORT DataManager
{
public:
	static DataManager* GetInstance();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	解析文件. </summary>
    ///     1. 获取路径下所有 .h 文件
    ///     2. 解析文件
    /// <remarks>	August295, 2022/9/7. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void FindFiles(const std::string& folder, std::map<std::string, std::map<std::string, bool>>& fileMap);
    void ParseFiles(const std::set<std::string>& includePaths);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	外部数据接口. </summary>
    ///     1. 获取统一资源路径
    ///     2. 获取所有解析类
    ///     3. 获取所有解析文件
    ///     4. 修改解析文件状态
    /// <remarks>	August295, 2022/9/7. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string GetResourcesPath();
    std::map<std::string, TypeParser>& GetTypeParserMap();
    std::map<std::string, std::map<std::string, bool>>& GetFileMap();
    void SetFileMap(const std::string& fileName, const std::string& structName, bool change);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	获取结构体类型列表. </summary>
    ///
    /// <remarks>	August295, 2022/9/8. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    std::list<std::string> GetTypeList();

private:
    DataManager();
    ~DataManager();

private:
    struct DataManagerPrivate;
    DataManagerPrivate* m_p;
};

#endif
