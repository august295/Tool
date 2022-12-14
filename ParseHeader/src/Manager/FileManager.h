#ifndef __FileManager_H__
#define __FileManager_H__

#include <string>
#include <vector>

#include "GlobalManager.h"

class MANAGER_EXPORT FileManager
{
public:
    static FileManager* GetInstance();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	获取文件内容. </summary>
    ///
    /// <remarks>	August295, 2022/9/8. </remarks>
    ///
    /// <param name="file">	文件名. </param>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<std::string> GetFileContent(const std::string& file);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	保存文件. </summary>
    ///
    /// <remarks>	August295, 2022/9/8. </remarks>
    ///
    /// <param name="file">		   	文件名. </param>
    /// <param name="fileLineList">	每行数据. </param>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void SaveFile(const std::string& file, const std::vector<std::string>& fileLineVec);

private:
    FileManager();
    ~FileManager();
};

#endif
