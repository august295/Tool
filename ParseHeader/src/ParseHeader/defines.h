#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   结构体成员变量信息
///     变量声明可以包含4个部分（以下语句为例：char* argv[2]; // 注释）：
///             - m_type:       char
///             - m_name:       argv
///             - m_comment:    注释
///             - m_is_pointer: true
///             - m_size_array: 2
///             - m_size_value: 8
/// </summary>
///
/// <remarks>	August295, 2022/9/5. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
struct StructDeclaration {
    std::string m_type;       ///< name of a data type, either basic type or user-defined type
    std::string m_name;       ///< variable name
    std::string m_comment;    ///< variable m_comment
    bool        m_is_pointer; ///< true when it's a pointer
    size_t      m_size_array; ///< array size: 0 for non-array
    size_t      m_size_value; ///< size in bytes
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>   枚举成员变量信息
///     变量声明可以包含2个部分（以下语句为例：ROOT = 0, // 注释）：
///             - m_name:       ROOT
///             - m_comment:    注释
///             - m_value:      0
/// </summary>
///
/// <remarks>	August295, 2022/9/5. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
struct EnumDeclaration {
    std::string m_name;    ///< variable name
    std::string m_comment; ///< variable m_comment
    int         m_value;   ///< variable value
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	数据标记类型. </summary>
///
/// <remarks>	August295, 2022/9/5. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
enum TokenTypes {
    kUnresolvedToken,

    // keywords
    kStructKeyword,
    kUnionKeyword,
    kEnumKeyword,
    kTypedefKeyword,
    kNamespaceKeyword,

    kBasicDataType,
    kAbstractType,
    kComplexType,
    kQualifier,

    // user-defined tokens
    kStructName,
    kUnionName,
    kEnumName,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	单字符标记的类型. </summary>
///
/// <remarks>	August295, 2022/9/5. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
enum SingleToken {
    kBlockStart = '{',
    kBlockEnd   = '}',
    kPoundSign  = '#',
    kComma      = ',',
    kSemicolon  = ';',
    kEqual      = '=',
    kSlash      = '/',
    kAsterisk   = '*',
    kQuotation  = '\"',
};

// 基本数据类型
static const std::vector<std::string> data_types = {
    "char",
    "short",
    "int",
    "long",
    "float",
    "double",
    "void",
    "bool",
};

// 解析时要忽略的限定符
static const std::vector<std::string> qualifiers = {
    "static",
    "const",
    "signed",
    "unsigned",
    "far",
    "extern",
    "volatile",
    "auto",
    "register",
    "inline",
    "__attribute__",
};

/// underline ('_') shouldn't be included as it can be part of an identifier
// static const std::string kTokenDelimiters = " \t#{[(<&|*>)]}?\':\",%!=/;+*$";
static const std::string kTokenDelimiters = " \t#{[(<&|*>)]}?,%!=/;+*$";

/// prefix that is used to make a fake identifier for anonymous struct/union/enum type
static const std::string kAnonymousTypePrefix = "_ANONYMOUS_";

/// name of padding field into a struct/union, for the purpose of alignment
static const std::string kPaddingFieldName = "_padding_field_";

#endif // _DEFINES_H_
