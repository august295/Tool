#ifndef _TYPE_PARSER_H_
#define _TYPE_PARSER_H_

#include <fstream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "defines.h"

#ifdef TYPEPARSER_LIBRARY
#define TYPEPARSER_EXPORT __declspec(dllexport)
#else
#define TYPEPARSER_EXPORT __declspec(dllimport)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	头文件解析类. </summary>
///
/// <remarks>	August295, 2022/9/5. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
class TYPEPARSER_EXPORT TypeParser {
public:
    TypeParser(void);
    ~TypeParser(void);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	初始化，读入基本数据，如关键字 / 限定符和基本数据类型大小. </summary>
    ///
    /// <remarks>	August295, 2022/9/5. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void Initialize();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	解析指定路径下文件. </summary>
    ///		1. 设置解析文件夹路径，与 2 一起使用
    ///		2. 指定路径解析
    ///		3. 指定文件解析
    /// <remarks>	August295, 2022/9/5. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void SetIncludePaths(std::set<std::string> paths);
    void ParseFiles();
    void ParseFile(const std::string& file);

    /**
     * @brief 解析处理好的文件字符串
     *
     * @param src      文件字符串
     */
    void ParseSource(const std::string& src);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	字符串处理. </summary>
    ///		1. 获取下一个标记-它可以是特殊字符，也可以是关键字/标识符
    ///		2. 获取下一行
    ///		3. 获取剩下的部分
    ///		4. 跳过当前行
    ///		5. 切割行获取标记
    /// <remarks>	August295, 2022/9/5. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool   GetNextToken(std::string src, size_t& pos, std::string& token, bool cross_line = true) const;
    bool   GetNextLine(std::string src, size_t& pos, std::string& line) const;
    bool   GetRestLine(const std::string& src, size_t& pos, std::string& line) const;
    void   SkipCurrentLine(const std::string& src, size_t& pos, std::string& line) const;
    size_t SplitLineIntoTokens(std::string line, std::vector<std::string>& tokens) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	具体解析函数. </summary>
    ///		1. 解析变量声明
    ///		2. 解析枚举成员声明
    ///		3. 解析赋值表达式
    ///		4. 解析预处理指令
    ///		5. 解析结构体或联合体
    ///		6. 解析枚举
    /// <remarks>	August295, 2022/9/5. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool ParseDeclaration(const std::string& line, StructDeclaration& decl) const;
    bool ParseEnumDeclaration(const std::string& line, int& last_value, EnumDeclaration& decl, bool& is_last_member) const;
    bool ParseAssignExpression(const std::string& line);
    void ParsePreProcDirective(const std::string& src, size_t& pos);
    bool ParseStructUnion(const bool is_struct, const bool is_typedef, const std::string& src, size_t& pos, StructDeclaration& decl, bool& is_decl);
    bool ParseEnum(const bool is_typedef, const std::string& src, size_t& pos, StructDeclaration& var_decl, bool& is_decl);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	结构体或联合体大小定义. </summary>
    ///		1. 构造默认变量信息
    ///		2. 使用填充字段填充结构以进行内存对齐
    ///		3. 计算联合体大小
    /// <remarks>	August295, 2022/9/5. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    StructDeclaration MakePadField(const size_t size) const;
    size_t            PadStructMembers(std::list<StructDeclaration>& members);
    size_t            CalcUnionSize(const std::list<StructDeclaration>& members) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	存储结构体或联合体的定义和大小. </summary>
    ///
    /// <remarks>	August295, 2022/9/5. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void StoreStructUnionDef(const bool is_struct, const std::string& type_name, std::list<StructDeclaration>& members);

private:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	获取指定路径下所有文件. </summary>
    ///
    /// <remarks>	August295, 2022/9/5. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void        FindHeaderFiles(std::string folder);
    std::string GetFile(std::string& filename) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	读取文件. </summary>
    ///		1. 获取文件数据转换为纯数据字符串
    ///		2. 去掉注释
    ///		3. 去掉尾部空格
    ///		4. 将 '\n' 换行合并为一行
    ///		5. 合并所有行
    /// <remarks>	August295, 2022/9/5. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string Preprocess(std::ifstream& ifs) const;
    void        StripComments(std::list<std::string>& lines) const;
    void        TrimLines(std::list<std::string>& lines) const;
    void        WrapLines(std::list<std::string>& lines) const;
    std::string MergeAllLines(const std::list<std::string>& lines) const;

    /**
     * @brief 解析结构体、联合体、枚举内部同行的注释
     *
     * @param src          文件字符串
     * @param pos          偏移
     * @param comment      注释
     * @return true        成功
     * @return false       失败
     */
    bool ParseComment(const std::string& src, size_t& pos, std::string& comment) const;

    /**
     * @brief 解析时跳过结构体、联合体内部函数
     *
     * @param src          文件字符串
     * @param pos          偏移
     * @return true        成功
     * @return false       失败
     */
    bool ParseSkipFunction(const std::string& src, size_t& pos) const;

    /**
     * @brief 获取作用域
     *
     * @param pos          偏移
     * @return std::string 作用域
     */
    std::string GetNamespace(size_t& pos) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>	工具函数. </summary>
    ///		1. 获取从行的指定位置开始的下一个标记，如果令牌不在行尾，pos将移动到令牌后面
    ///		2. 返回true表示它是一个空标记，或者是一个可以忽略的限定符
    ///		3. 从已知关键字/限定符或基本/使用定义类型查询标记类型
    ///		4. 检查令牌是数字还是可以转换为数字
    ///		5. 获取类型大小
    ///		6. 转储提取的类型定义
    /// <remarks>	August295, 2022/9/5. </remarks>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string GetNextToken(const std::string line, size_t& i) const; // TODO: std::string ignore=" \t"
    bool        IsIgnorable(std::string token) const;
    TokenTypes  GetTokenType(const std::string& token) const;
    bool        IsNumericToken(const std::string& token, long& number) const;
    size_t      GetTypeSize(const std::string& type) const;
    void        DumpTypeDefs() const;

public:
    static const char   EOL         = '$'; ///< end of line, used to delimit the source lines within a std::string
    static const size_t kAlignment_ = 4;   ///< alignment @toto: make this changeable
    static const size_t kWordSize_  = 4;   ///< size of a machine word on a 32-bit system

public:
    /// namespace
    std::vector<std::tuple<std::string, size_t, size_t>> namespaces_;

    /// external input
    std::set<std::string> include_paths_;

    /// basic data that're needed in parsing
    std::set<std::string>             basic_types_;
    std::set<std::string>             qualifiers_;
    std::map<std::string, TokenTypes> keywords_;

    /// header files to parse
    /// key     - filename with relative/absolute path
    /// bool    - whether the file is parsed
    std::map<std::string, bool> header_files_;

    /// Size of C data types and also user-defined struct/union types
    /// @note All enum types have fixed size, so they're not stored
    std::map<std::string, size_t> type_sizes_;

    /// Parsing result - extracted type definitions
    /// for below 3 maps:
    /// key     - type name
    /// value   - type members

    /// struct definitons
    std::map<std::string, std::list<StructDeclaration>> struct_defs_;

    /// union definitions
    std::map<std::string, std::list<StructDeclaration>> union_defs_;

    /// enum definitions
    std::map<std::string, std::list<EnumDeclaration>> enum_defs_;

    /// constants and macros that have integer values
    /// key     - constant/macro name
    /// value   - a integer (all types of number are cast to long type for convenience)
    std::map<std::string, long> const_defs_;
};

#endif // _TYPE_PARSER_H_
