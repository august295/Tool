#ifndef __DATAMANAGER_H__
#define __DATAMANAGER_H__

#include <list>
#include <map>
#include <set>
#include <string>

#include "GlobalManager.h"

class TypeParser;

class MANAGER_EXPORT DataManager {
public:
    static DataManager* GetInstance();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	解析文件. </summary>
    ///     1. 获取路径下所有 .h 文件
    ///     2. 解析多路径下所有文件
    ///     3. 解析单个文件
    /// <remarks>	August295, 2022/9/7. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void FindFiles(const std::string& folder, std::map<std::string, std::map<std::string, bool>>& fileMap);
    void ParseFiles(const std::set<std::string>& includePaths);
    void ParseFile(const std::string& fileName);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	获取数据. </summary>
    ///     1. 获取统一资源路径
    ///     2. 获取所有解析类
    ///     3. 获取所有解析文件
    ///     4. 获取所有类型列表
    /// <remarks>	August295, 2022/9/7. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string                                         GetResourcesPath();
    std::map<std::string, TypeParser>&                  GetTypeParserMap();
    std::map<std::string, std::map<std::string, bool>>& GetFileUpdateMap();
    std::list<std::string>                              GetTypeList();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	设置数据. </summary>
    ///     1. 修改文件状态
    /// <remarks>	August295, 2022/9/8. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void SetFileUpdateMap(const std::string& fileName, const std::string& structName, bool change);

private:
    DataManager();
    ~DataManager();

private:
    struct DataManagerPrivate;
    DataManagerPrivate* m_p;
};

#endif
