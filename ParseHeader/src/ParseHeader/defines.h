#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	结构体成员变量信息
/// 	变量声明可以包含4个部分（以下语句为例：char* argv[2]）：
/// 	   - data_type:     char
/// 	   - var_name:      argv
/// 	   - array_size:    2
/// 	   - is_pointer:    true
/// 	   - var_size:		8
/// 	   - comment:		注释
/// 	这里只支持一个维度数组，但很容易扩展.
/// </summary>
///
/// <remarks>	李俊峰, 2022/9/5. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    std::string data_type;  ///< name of a data type, either basic type or user-defined type
    std::string var_name;   ///< variable name
    size_t      array_size; ///< array size: 0 for non-array
    bool        is_pointer; ///< true when it's a pointer
    size_t      var_size;   ///< size in bytes
    std::string comment;
} VariableDeclaration;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	数据标记类型. </summary>
///
/// <remarks>	李俊峰, 2022/9/5. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
enum TokenTypes
{
    kUnresolvedToken,

    // keywords
    kStructKeyword,
    kUnionKeyword,
    kEnumKeyword,
    kTypedefKeyword,

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
/// <remarks>	李俊峰, 2022/9/5. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
enum SingleToken
{
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
static const std::string kTokenDelimiters     = " \t#{[(<&|*>)]}?\':\",%!=/;+*$";

/// prefix that is used to make a fake identifier for anonymous struct/union/enum type
static const std::string kAnonymousTypePrefix = "_ANONYMOUS_";

/// name of padding field into a struct/union, for the purpose of alignment
static const std::string kPaddingFieldName    = "_padding_field_";

#endif // _DEFINES_H_
