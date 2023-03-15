#include <algorithm>
#include <iostream>
#include <math.h>
#include <regex>

#include <spdlog/spdlog.h>

#ifdef WIN32
#include "dirent.h" // opendir/readdir, @see http://www.softagalleria.net/download/dirent/
#else
#include <dirent.h>    // DIR
#include <math.h>      // ceil
#include <sys/stat.h>  // S_ISDIR
#include <sys/types.h> // opendir/readdir
#endif

#include "TypeParser.h"
#include "utility.h"

TypeParser::TypeParser(void)
{
    spdlog::set_level(spdlog::level::err);
    Initialize();
}

TypeParser::~TypeParser(void)
{
}

void TypeParser::Initialize()
{
    basic_types_ = std::set<std::string>(data_types.begin(), data_types.end());
    qualifiers_  = std::set<std::string>(qualifiers.begin(), qualifiers.end());

    // keywords that we care
    keywords_["struct"]    = kStructKeyword;
    keywords_["union"]     = kUnionKeyword;
    keywords_["enum"]      = kEnumKeyword;
    keywords_["typedef"]   = kTypedefKeyword;
    keywords_["namespace"] = kNamespaceKeyword;
    keywords_["using"]     = kUsingKeyword;

    // sizes of basic data types on 32-bit system, in bytes
    for (std::set<std::string>::const_iterator it = basic_types_.begin(); it != basic_types_.end(); ++it) {
        type_sizes_[*it] = kWordSize_;
    }
    type_sizes_["void"]           = 0;
    type_sizes_["bool"]           = sizeof(bool);
    type_sizes_["char"]           = sizeof(char);
    type_sizes_["short"]          = sizeof(short);
    type_sizes_["unsigned char"]  = sizeof(unsigned char);
    type_sizes_["unsigned short"] = sizeof(unsigned short);
    type_sizes_["unsigned int"]   = sizeof(unsigned int);
    type_sizes_["unsigned long"]  = sizeof(unsigned long);
}

/// Set header file includsion path
/// @param[in]  paths   a std::set of header file inclusion paths, both relative/absolute paths are okay
///
void TypeParser::SetIncludePaths(const std::set<std::string> paths)
{
    include_paths_ = paths;
}

/// Parse all header files under including paths
///
/// @note current folder will be added by default
///
void TypeParser::ParseFiles()
{
    // TODO: add current folder by default
    // since include_paths_ is a std::set, it won't be added duplicately
    // include_paths_.insert(".");
    for (std::set<std::string>::const_iterator it = include_paths_.begin(); it != include_paths_.end(); ++it) {
        FindHeaderFiles(*it);
        for (std::map<std::string, bool>::const_iterator it = header_files_.begin(); it != header_files_.end(); ++it) {
            ParseFile(it->first);
        }
    }
}

/// Parse a header file
///
/// It can be called individually to parse a specified header file
///
/// @param[in]  file    filename with either absolute or relative path
///
void TypeParser::ParseFile(const std::string& file)
{
    std::ifstream ifs(file, std::ios::in);
    if (ifs.fail()) {
        SPDLOG_ERROR("Failed to open file - " + file);
        return;
    }

    // flag to true before parsing the file so that it won't be parsed duplicately
    header_files_[file] = true;
    spdlog::debug("Parsing file - " + file);

    std::string str = Preprocess(ifs);
    ParseSource(str);
}

/*
 * Recursively find all the header files under specified folder
 * and store them into header_files_
 *
 * Folder name can end with either "\\" or "/", or without any
 *
 * Assumption: header files end with ".h"
 */
void TypeParser::FindHeaderFiles(std::string folder)
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
                    spdlog::info("Searching folder: " + name);
                    FindHeaderFiles(name);
                } else {
                    if (name.length() > 2 && name.substr(name.length() - 2, 2).compare(".h") == 0) {
                        header_files_[name] = false; // "false" means not yet parsed
                        spdlog::debug("Found header file: " + name);
                    } else {
                        spdlog::info("Ignoring file: " + name);
                    }
                }
            } else {
                SPDLOG_ERROR("failed to stat file/folder: " + name);
            }
        }
        closedir(dir);
    } else {
        SPDLOG_ERROR("failed to open folder: " + folder);
    }
}

// search a file from the include paths
// when found, return full path of the file and also update the input argument
// else return empty std::string - it's up to the caller to check the return value
std::string TypeParser::GetFile(std::string& filename) const
{
    DIR*           dir;
    struct dirent* ent;
    std::string    path_name;

    for (std::set<std::string>::const_iterator it = include_paths_.begin(); it != include_paths_.end(); ++it) {
        std::string folder = *it;
        if ((dir = opendir(folder.c_str())) == NULL)
            continue;

        while ((ent = readdir(dir)) != NULL) {
            std::string name(ent->d_name);
            if (name.compare(filename) == 0) {
                path_name = folder + "/" + filename;
                break;
            }
        }

        closedir(dir);
        if (!path_name.empty())
            break;
    }
    return (filename = path_name);
}

std::string TypeParser::Preprocess(std::ifstream& ifs) const
{
    std::string            line;
    std::list<std::string> lines;

    while (ifs.good()) {
        getline(ifs, line);

        if (!trim(line).empty()) {
            lines.push_back(line);
        }
    }
    //    StripComments(lines);
    WrapLines(lines);
    return MergeAllLines(lines);
}

/// Strip all comments from code lines
///
/// - either line comment or comment blocks will be removed
/// - all special cases (like multiple comment blocks in one line) can be handled perfectly
///     except fake comments within quotation marks (like str = "/* fake comment */ // non-comment";)
///     but this doesn't impact the parsing
///
/// @param[in,out]  lines   lines of source code, comments will be removed directly from the lines
void TypeParser::StripComments(std::list<std::string>& lines) const
{
    bool        is_block   = false; // whether a comment block starts
    bool        is_changed = false; // whether current line is changed after removing comments
    std::string line;
    size_t      pos = 0;

    std::list<std::string>::iterator block_start, it = lines.begin();
    while (it != lines.end()) {
        is_block = is_changed = false;
        line                  = *it;
        pos                   = 0;
        spdlog::info("parsing line: [" + line + "]");

        // search comment start
        while (std::string::npos != (pos = line.find(kSlash, pos))) {
            if (line.length() <= pos + 1)
                break; // the 1st '/' is at the end of line, so not a comment

            switch (line.at(pos + 1)) {
            case kSlash: // line comment
                line.erase(pos);
                is_changed = true;
                break;
            case kAsterisk: // comment block
                is_block   = true;
                is_changed = true;
                do {
                    size_t found = line.find("*/", pos + 2);
                    if (std::string::npos != found) {
                        line.erase(pos, found - pos + 2);
                        is_block = false;
                    } else {
                        line.erase(pos);
                        is_block = true;
                        break;
                    }
                } while (!is_block && std::string::npos != (pos = line.find("/*", pos)));
                // loop for possible multiple comment blocks on one line
                break;
            default:
                pos++; // might be other '/' after this one
            }
        }

        if (!is_changed) {
            ++it;
            continue;
        } else {
            if (!line.empty())
                lines.insert(it, line);

            it = lines.erase(it);
        }

        if (is_block) {
            block_start = it;
            while (it != lines.end()) {
                line = *it;
                if (std::string::npos != (pos = line.find("*/"))) {
                    lines.erase(block_start, ++it);

                    line.erase(0, pos + 2);
                    if (!line.empty()) {
                        it = lines.insert(it, line); // insert rest part for checking of possible followed comments
                    }

                    is_block = false;
                    break;
                }
                ++it;
            }

            if (is_block) {
                SPDLOG_ERROR("Unclosed comment block exists");
            }
        }
    }
}

// Merge wrapped lines into one line
// A line will be deemed as line wrap only when the last character is '\'
void TypeParser::WrapLines(std::list<std::string>& lines) const
{
    std::string                      line;
    std::list<std::string>::iterator first;
    std::list<std::string>::iterator it = lines.begin();

    while (it != lines.end()) {
        first = it;
        line  = *it;
        if (line.empty()) {
            SPDLOG_ERROR("wrap line is empty");
            exit(-1);
        }

        while ('\\' == line[line.length() - 1] && ++it != lines.end()) {
            line = line.substr(0, line.length() - 1);
            line = rtrim(line) + (*it);
        }

        if (it == lines.end()) {
            SPDLOG_ERROR("Bad syntax: wrap line at last line");
            break;
        }

        if (it != first) {
            lines.insert(first, line); // insert merged std::string before "first" without invalidating the iterators

            ++it; // increase so that current line can be erased from the std::list
            it = lines.erase(first, it);
        } else {
            ++it;
        }
    }
}

// Merge all lines into a std::string for easier parsing
//    with lines delimited by EOL sign
//
// NOTE: if there're characters like  ',' or kSemicolon within a line,
//    split the line into multiple lines
std::string TypeParser::MergeAllLines(const std::list<std::string>& lines) const
{
    std::string src, line, part;
    size_t      pos;

    std::list<std::string>::const_iterator it = lines.begin();
    while (it != lines.end()) {
        line = *it;
        if (line.empty()) {
            SPDLOG_ERROR("merge line is empty");
            exit(-1);
        }

        if ('#' == line.at(0)) { // don't  split pre-processing line
            src += line + EOL;
        } else {
            while (std::string::npos != (pos = line.find_first_of(",;"))) {
                if (pos == line.length() - 1) {
                    break;
                } else { // split line
                    part = line.substr(0, pos + 1);
                    src += trim(part) + EOL; // trim splited lines
                    line = line.substr(++pos);
                }
            }
            src += line + EOL;
        }
        ++it;
    }
    return src;
}

// Trim trailing spaces for each line
void TypeParser::TrimLines(std::list<std::string>& lines) const
{
    std::string                      line;
    std::list<std::string>::iterator it = lines.begin();

    while (it != lines.end()) {
        line = *it;
        if (rtrim(line).compare(*it) != 0) {
            lines.insert(it, line);
            it = lines.erase(it);
        } else {
            ++it;
        }
    }
}

// Get next token starting from the specified position of the line
// pos will be moved to after the token if token not at line end
//
// Assuming not all of the rest characters are blanks as all lines are trimed before
// Note:
// Among the many punctuations that can be used in C language (like #[(<&|*>)]} ? \' : \", % != /;)
//   only '_' can be part of a token here
std::string TypeParser::GetNextToken(const std::string line, size_t& pos) const
{
    if (pos >= line.length())
        return "";

    // skip blanks
    while (isspace(line[pos]))
        pos++;
    size_t start = pos;

    char ch;
    while (pos < line.length()) {
        ch = line[pos];
        if (isspace(ch) || (ispunct(ch) && '_' != ch))
            break;

        ++pos; // move to the next character
    }

    spdlog::debug("Next token: " + line.substr(start, pos - start));
    return (start == pos) ? "" : line.substr(start, pos - start);
}

// return true is it's an empty token or it's a qualifer that can be ignored
bool TypeParser::IsIgnorable(std::string token) const
{
    if (token.empty()) {
        return true;
    } else {
        return (qualifiers_.end() != qualifiers_.find(token));
    }
}

/// Query token type from known keywords/qualifiers or basic/use-defined types
///
/// @param[in]  token   a token
/// @return the corresponding token type, or kUnresolvedToken if not found
///
TokenTypes TypeParser::GetTokenType(const std::string& token) const
{
    if (keywords_.end() != keywords_.find(token)) {
        return keywords_.at(token);
    } else if (qualifiers_.end() != qualifiers_.find(token)) {
        return kQualifier;
    } else if (basic_types_.end() != basic_types_.find(token)) {
        return kBasicDataType;
    } else if (struct_defs_.end() != struct_defs_.find(token)) {
        return kStructName;
    } else if (union_defs_.end() != union_defs_.find(token)) {
        return kUnionName;
    } else if (enum_defs_.end() != enum_defs_.find(token)) {
        return kEnumName;
    } else {
        return kUnresolvedToken;
    }
}

/// Check whether the token is a number or can be translated into a number
///
/// @param[in]  token   a token
/// @param[out] number  the token's numeric value
/// @return true if 1) the token is a number, or
///                 2) the token is a macro that have a number as its value, or
///                 3) the token is a const variable that has been assigned to a number
bool TypeParser::IsNumericToken(const std::string& token, long& number) const
{
    if (token.empty()) {
        return false;
    }
    bool ret = true;
    // the token is '0'
    if (token == "0") {
        number = 0;
    } else {
        // stol not supported by gcc, re-write the code to use strtol
        number = strtol(token.c_str(), NULL, 0);
        if (0L == number) { // no valid conversion could be performed
            // the token is not a number, then check whether it can be translated into a number
            if (const_defs_.find(token) != const_defs_.end()) {
                number = const_defs_.at(token);
            } else {
                spdlog::debug("Cannot parse token <" + token + "> into a number");
                ret = false;
            }
        }
    }
    return ret;
}

/// Get size of a type (in bytes)
///
/// @param[in]  type    name of a data type, including both C data types and user-defined struct/union/enum types
/// @return -1 if is data type is unknown, else return actual type length
///
/// @note Shouldn't return 0 for unknown data type since "void" type has zero length
///
size_t TypeParser::GetTypeSize(const std::string& type) const
{
    if (type_sizes_.find(type) != type_sizes_.end()) {
        return type_sizes_.at(type);
    } else if (enum_defs_.find(type) != enum_defs_.end()) {
        return sizeof(int);
    } else {
        SPDLOG_ERROR("Unknown data type - " + type);
        return -1;
    }
}

/// Get next token - it can either be a special character, or a keyword/identifier
///
/// @param[in]      src     source code
/// @param[in,out]  pos     start position to parse the code;
///                         will be updated to the first char after the token after this method is called
/// @param[out]     token   the next token after @var pos
/// @param[in]      cross_line  true by default, false is only used for parsing pre-processing directives
/// @return         false only when file end is reached
///
/// @note When cross_line is false, only get next token from current line where @var pos resides
/// @note Qualifiers defined in @var qualifiers_ are skipped as they don't matter
///
bool TypeParser::GetNextToken(std::string src, size_t& pos, std::string& token, bool cross_line) const
{
    // cross_line=false, only check token from current line
    if (!cross_line) {
        size_t p = src.find(EOL, pos);
        if (std::string::npos != p)
            src = src.substr(0, p);
    }

    // skip leading blanks or EOL
    while (pos < src.length() && (isspace(src[pos]) || EOL == src[pos]))
        pos++;

    if (pos >= src.length()) {
        token.clear();
        return false;
    }

    size_t start = pos;
    size_t ptk   = src.find_first_of(kTokenDelimiters, start);
    if (std::string::npos == ptk) {
        token = src.substr(start);
        pos   = src.length();
    } else if (start == ptk) {
        pos   = ptk + 1;
        token = std::string(1, src[ptk]);
    } else {
        pos   = ptk;
        token = src.substr(start, ptk - start);
    }

    // skip possible empty or ignorable tokens
    while (!token.empty() && IsIgnorable(token))
        GetNextToken(src, pos, token);

    return (token.empty() ? false : true);
}

/// Get the next line
/// @param[in,out]  pos     start position to parse the code;
///                         will be updated to end of next line after this method is called
/// @return false only when current line is the last line
bool TypeParser::GetNextLine(std::string src, size_t& pos, std::string& line) const
{
    size_t start = src.find(EOL, pos);
    if (std::string::npos == start) {
        pos = src.length();
        line.clear();
        return false;
    }

    ++start;
    size_t end = src.find(EOL, start);
    if (std::string::npos == end) {
        line = src.substr(start);
        pos  = src.length();
    } else {
        if (end == start) {
            SPDLOG_ERROR("next line is empty");
            exit(-1);
        }
        line = src.substr(start, end - start);
        pos  = end;
    }

    if (trim(line).empty()) {
        SPDLOG_ERROR("trim line is empty");
        exit(-1);
    }
    return true;
}

/// skip current line that @var pos resides
///
/// @param[in]      src     source code
/// @param[in,out]  pos     start position to parse the code;
///                         will be updated to the first char of next line after this method is called
/// store the line content into "line"
void TypeParser::SkipCurrentLine(const std::string& src, size_t& pos, std::string& line) const
{
    if (pos >= src.length()) {
        SPDLOG_ERROR("SkipCurrentLine() - std::string offset larger than its length");
        line.clear();
        return;
    }

    size_t start = src.rfind(EOL, pos);
    size_t end   = src.find(EOL, pos);
    if (end == std::string::npos) {
        line = src.substr(start + 1); // it's ok even if start = std::string::npos as it equals -1
        pos  = src.length();
    } else if (end == pos) {
        start = src.rfind(EOL, pos - 1);
        line  = src.substr(start + 1, end - start - 1); // it's ok even if start = std::string::npos
        pos++;
    } else {
        line = src.substr(start + 1, end - start - 1);
        pos  = end + 1;
    }
}

/// Parse pre-processing directives
///
/// Only below directives with the exact formats are supported:
/// 1) #include "<header filename>"
/// 2) #define <macro name> <number>
/// For others, the whole line will be skipped
///
/// @param src  source code
/// @param pos  start position to parse the code, pointing to the next char after '#';
///             it will be updated to new position when the code is parsed
///
/// @note when a header file inclusion directive is met, the header file is parsed
/// immediately to ensure the correct parsing sequence
///
void TypeParser::ParsePreProcDirective(const std::string& src, size_t& pos)
{
    std::string token, last_token, line;
    long        number;

    GetNextToken(src, pos, token);
    if (0 == token.compare("include")) {
        if (!GetNextToken(src, pos, token, false)) {
            SPDLOG_ERROR("GetNextToken() error");
            exit(-1);
        }

        // only handle header file included with ""
        if (kQuotation == token[token.length() - 1]) {
            // get included header file name
            if (!GetNextToken(src, pos, token, false)) {
                SPDLOG_ERROR("GetNextToken() error");
                exit(-1);
            }

            // parse the header file immediately
            ParseFile(token);

            // ignore the other quotation marks
            SkipCurrentLine(src, pos, line);
        } else {
            // ignore angle bracket (<>)
            SkipCurrentLine(src, pos, line);
            spdlog::info("Skip header file included by <> - " + line);
        }
    } else if (0 == token.compare("define")) {
        if (!GetNextToken(src, pos, last_token, false)) {
            SPDLOG_ERROR("GetNextToken() error");
            exit(-1);
        }

        if (GetNextToken(src, pos, token, false) && IsNumericToken(token, number)) {
            const_defs_.insert(make_pair(last_token, number));
        } else {
            SkipCurrentLine(src, pos, line);
            spdlog::info("Ignore define - " + line);
        }
    } else {
        SkipCurrentLine(src, pos, line);
        spdlog::info("Skip unsupported pre-processing line - " + line);
    }
}

void TypeParser::ParseSource(const std::string& src)
{
    size_t      pos = 0;
    std::string token, last_token, line;
    bool        is_typedef = false;
    TokenTypes  type;

    StructDeclaration decl;
    bool              is_decl = false;

    std::string type_name;
    while (GetNextToken(src, pos, token)) {
        if (token.length() == 1) {
            switch (token[0]) {
            case kPoundSign:
                ParsePreProcDirective(src, pos);
                break;
            case kBlockStart:
                break;
            case kBlockEnd:
                break;
            case kSemicolon:
                break;
            case kSlash:
                --pos;
                ParseComment(src, pos, comment_);
                break;
            default:
                SkipCurrentLine(src, pos, line);
                spdlog::debug("Character '" + token + "' unexpected, ignore the line");
            }
        } else {
            type = GetTokenType(token);
            switch (type) {
            case kStructKeyword:
            case kUnionKeyword:
                if (!ParseStructUnion((kStructKeyword == type) ? true : false, is_typedef, src, pos, decl, is_decl) && is_decl) {
                    SPDLOG_ERROR("Bad syntax for struct/union variable declaration");
                    return;
                }
                // reset is_typedef
                is_typedef = false;
                break;
            case kEnumKeyword:
                if (!ParseEnum(is_typedef, src, pos, decl, is_decl) || is_decl) {
                    SPDLOG_ERROR("Bad syntax for nested enum variable declaration");
                    return;
                }
                is_typedef = false;
                break;
            case kTypedefKeyword:
                is_typedef = true;
                break;
            case kBasicDataType:
                // only (const) global variable are supported
                if (!GetRestLine(src, pos, line)) {
                    SPDLOG_ERROR("GetRestLine() error");
                    exit(-1);
                }
                if (!ParseAssignExpression(line)) {
                    spdlog::debug("Expression not supported - " + line);
                }
                break;
            case kNamespaceKeyword:
                GetNextToken(src, pos, token);
                namespaces_.push_back(std::make_tuple(token, pos, 0));
                break;
            case kUsingKeyword:
                SkipCurrentLine(src, pos, line);
                break;
            default:
                break;
            }
        }
    }
}

/// Parse enum block
bool TypeParser::ParseEnum(const bool is_typedef, const std::string& src, size_t& pos, StructDeclaration& var_decl, bool& is_decl)
{
    EnumDeclaration            member;
    std::list<EnumDeclaration> members;
    std::string                line, token, next_token;
    std::string                type_name, type_alias;

    int  last_value     = -1;
    bool is_last_member = false;

    if (src.empty() || pos > src.length()) {
        SPDLOG_ERROR("ParseEnum() error");
        exit(-1);
    }

    size_t start = pos; // store the original position for next guess

    // peek rest of current line starting from "pos"
    if (!GetRestLine(src, pos, line)) {
        if (!GetNextLine(src, pos, line)) {
            SPDLOG_ERROR("GetNextLine() error");
            exit(-1);
        }
    }

    // it might be just a simple enum variable declaration like: enum Home home;
    if (ParseDeclaration(line, var_decl)) {
        return (is_decl = true);
    }

    // else, next token should be either '{' or a typename followed by '{'
    pos = start; // reset the position
    if (!GetNextToken(src, pos, token)) {
        SPDLOG_ERROR("GetNextToken() error");
        exit(-1);
    }
    if (kBlockStart != token.at(0)) {
        type_name = token;
        if (!GetNextToken(src, pos, token) || kBlockStart != token.at(0)) {
            SPDLOG_ERROR("GetNextToken() error");
            exit(-1);
        }
    }

    // the following part should be:
    // 1) enum member declarations within the block
    // 2) something out of the block like "} [<type alias|var>];"
    while (GetNextToken(src, pos, token)) {
        if (kBlockEnd == token.at(0)) { // reach block end
            // process rest part after block end
            start = 1;
            if (!GetNextToken(src, pos, token)) {
                SPDLOG_ERROR("GetNextToken() error");
                exit(-1);
            }

            std::string space = GetNamespace(pos);
            type_name         = space + type_name;
            if (is_typedef) {
                // format 1
                if (!GetNextToken(src, pos, next_token) || kSemicolon != next_token.at(0)) {
                    SPDLOG_ERROR("GetNextToken() error");
                    exit(-1);
                }
                is_decl                 = false;
                type_alias              = space + token;
                enum_defs_[type_alias]  = members;
                type_sizes_[type_alias] = sizeof(int);
                type_name               = type_alias;
            } else {
                // non-typedef
                if (kSemicolon == token.at(0)) {
                    // format 2
                    is_decl = false;
                    if (type_name.empty()) {
                        SPDLOG_ERROR("type_name is empty");
                        exit(-1);
                    }
                    enum_defs_[type_name]  = members;
                    type_sizes_[type_name] = sizeof(int);
                } else {
                    // token must be part of a variable declaration
                    // so it must be format 3 or 4

                    if (type_name.empty()) {
                        // format 4: anonymous type
                        // generate a unique random name for this type so that it can be stored into std::map
                        do {
                            type_name = kAnonymousTypePrefix + rands();
                        } while (enum_defs_.end() != enum_defs_.find(type_name));
                    }

                    is_decl                = true;
                    enum_defs_[type_name]  = members;
                    type_sizes_[type_name] = sizeof(int);

                    if (!GetRestLine(src, pos, line)) {
                        if(!GetNextLine(src, pos, line)){
                            SPDLOG_ERROR("GetNextLine() error");
                            exit(-1);
                        }
                    }

                    // for easier parsing, make a declaration by adding the <type_name> before <var>
                    line = type_name + " " + token + " " + line;
                    if (!ParseDeclaration(line, var_decl)) {
                        SPDLOG_ERROR("Bad syntax for enum type of variable declaration after {} block");
                        return false;
                    }
                }
            }

            // get comment
            comments_[type_name] = comment_;
            comment_.clear();

            // break as block ends
            break;
        } else {
            // skip useless comments
            if (token.at(0) == '/') {
                ParseCommentSkip(src, pos);
                continue;
            }

            // parse enum member declarations
            if (is_last_member) {
                SPDLOG_ERROR("Bad enum member declaration in last line");
                return false;
            }

            // Note: the last enum memeber can only have one token, so the rest line can be empty here!
            GetRestLine(src, pos, line);

            line = token + " " + line;
            if (!ParseEnumDeclaration(line, last_value, member, is_last_member)) {
                SPDLOG_ERROR("Unresolved enum member declaration syntax");
                return false;
            }

            // parse comments
            size_t index = pos;
            GetNextToken(src, index, token);
            if (token == "/") {
                ParseComment(src, pos, member.m_comment);
            } else {
                member.m_comment = "";
            }

            spdlog::info("Add enum member: " + member.m_name);
            members.push_back(member);
        }
    }

    return true;
}
/// Parse struct/union definition or declaration
///
/// @param[in]  is_typedef  whether there's the "typedef" keyword before this statement
/// @param[in]  src         source code to parse
/// @param[in]  pos         position
/// @param[out] var_decl    will be updated only when @var is_decl is true
/// @param[out] is_decl     true if it's a variable declaration (with/without nested type definition);
///                         false if it's a pure type definition
/// return true if it's a struct/union definition or declaration
///
//     starting from the next character after the "struct" or "union" keyword
//     to end of the definition/declaration
//
/// it might be called recursively to parse nested structure of struct/union
//
/// it supports below formats:
///     1) type definition: typedef <type> [<type_name>] {....} <type_alias>;
///     2) type definition:         <type> <type_name> {....};
///     3) var declaration:         <type> <type_name> {....} <var>;
///     4) var declaration:         <type>             {....} <var>;   // anonymous type
///     5) var declaration:        [<type>] <type_name> <var>;  // type_name is defined elsewhere
///     where, <type> refers to either "struct" or "union" keyword
///            <var> here can be as complicated as "*array[MAX_SIZE]"
//
// after calling this function:
//     struct/union definitons will be stored into class member struct_defs_ or union_defs_
//     pos will point to the next kSemicolon following the block end '}',
//         or equal to src.length() when reaching file end - bad syntax
//     is_decl returns:
//     1) false for definition format 1 and 2;
//     2) true for declaration format 3-5 and "var_decl" argument being updated
///
bool TypeParser::ParseStructUnion(const bool is_struct, const bool is_typedef, const std::string& src, size_t& pos, StructDeclaration& var_decl, bool& is_decl)
{
    StructDeclaration            member;
    std::list<StructDeclaration> members;
    std::string                  line, token, next_token;
    std::string                  type_name, type_alias;

    if (src.empty() || pos > src.length()) {
        SPDLOG_ERROR("ParseStructUnion() error");
        exit(-1);
    }

    size_t start = pos; // store the original position for next guess

    // peek rest of current line starting from "pos"
    if (!GetRestLine(src, pos, line)) {
        if (!GetNextLine(src, pos, line)) {
            SPDLOG_ERROR("GetNextLine() error");
            exit(-1);
        }
    }

    // it might be just a simple struct/union variable declaration as format 5
    if (ParseDeclaration(line, var_decl)) {
        return (is_decl = true);
    }

    // else, next token should be either '{' or a typename followed by '{'
    pos = start; // reset the position
    if (!GetNextToken(src, pos, token)) {
        SPDLOG_ERROR("GetNextToken() error");
        exit(-1);
    }
    if (kBlockStart != token.at(0)) {
        type_name = token;
        if (!GetNextToken(src, pos, token) || kBlockStart != token.at(0)) {
            SPDLOG_ERROR("GetNextToken() error");
            exit(-1);
        }
    }

    // the following part should be:
    // 1) struct/union member declarations within the block
    // 2) something out of the block like "} [<type alias|var>];"
    while (GetNextToken(src, pos, token)) {
        if (kBlockEnd == token.at(0)) {
            // process rest part after block end
            start = 1;
            if (!GetNextToken(src, pos, token)) {
                SPDLOG_ERROR("GetNextToken() error");
                exit(-1);
            }

            std::string space = GetNamespace(pos);
            type_name         = space + type_name;
            if (is_typedef) {
                // format 1     get all typedef aliases
                std::vector<std::string> aliasVec;
                aliasVec.push_back(token);
                while (GetNextToken(src, pos, next_token)) {
                    if (kSemicolon == next_token.at(0)) {
                        break;
                    }
                    if (kComma != next_token.at(0)) {
                        aliasVec.push_back(next_token);
                    }
                }
                is_decl = false;
                // anonymous struct
                if (type_name == space) {
                    StoreStructUnionDef(is_struct, aliasVec.at(0), members);
                } else {
                    StoreStructUnionDef(is_struct, type_name, members);
                }
                for (auto alias : aliasVec) {
                    type_alias = space + alias;
                    struct_aliases_.emplace(type_name, type_alias);
                }
            } else { // non-typedef
                if (kSemicolon == token.at(0)) {
                    // format 2
                    is_decl = false;
                    if (type_name.empty()) {
                        SPDLOG_ERROR("type_name is empty");
                        exit(-1);
                    }
                    StoreStructUnionDef(is_struct, type_name, members);
                } else {
                    // token must be part of a variable declaration
                    // so it must be format 3 or 4
                    if (type_name.empty()) {
                        // format 4: anonymous type
                        // generate a unique random name for this type so that it can be stored into std::map
                        do {
                            type_name = kAnonymousTypePrefix + rands();
                        } while (struct_defs_.end() != struct_defs_.find(type_name));
                    }

                    is_decl = true;
                    StoreStructUnionDef(is_struct, type_name, members);

                    if (!GetRestLine(src, pos, line)) {
                        if (!GetNextLine(src, pos, line)) {
                            SPDLOG_ERROR("GetNextLine() error");
                            exit(-1);
                        }
                    }

                    // for easier parsing, make a declaration by adding the <type_name> before <var>
                    line = type_name + " " + token + " " + line;
                    if (!ParseDeclaration(line, var_decl)) {
                        SPDLOG_ERROR("Bad syntax for struct/union type of variable declaration after {} block");
                        return false;
                    }
                }
            }

            // get comment
            comments_[type_name] = comment_;
            comment_.clear();

            // break as block ends
            break;
        } else { // parse struct/union member declarations
            TokenTypes type = GetTokenType(token);

            if (kStructKeyword == type || kUnionKeyword == type) {
                // a nested struct/union variable declaration
                // bool is_declare;
                // StructDeclaration var_decl;
                if (!ParseStructUnion((kStructKeyword == type) ? true : false, false, src, pos, member, is_decl)) {
                    SPDLOG_ERROR("Bad syntax for nested struct/union variable declaration");
                    return false;
                }

                // TODO: also check position here
                if (!is_decl) {
                    SPDLOG_ERROR("it is not a struct declaration");
                    exit(-1);
                }
                members.push_back(member);
            } else if (kEnumKeyword == type) {
                if (!ParseEnum(false, src, pos, member, is_decl)) {
                    SPDLOG_ERROR("Bad syntax for nested enum variable declaration");
                    return false;
                }

                // TODO: also check position here
                if (!is_decl) {
                    SPDLOG_ERROR("it is not a enum declaration");
                    exit(-1);
                }
                members.push_back(member);
            } else {
                // skip useless comments
                if (token.at(0) == '/') {
                    ParseCommentSkip(src, pos);
                    continue;
                }

                // regular struct/union member declaration, including format 5
                if (!GetRestLine(src, pos, line)) {
                    if (!GetNextLine(src, pos, line)) {
                        SPDLOG_ERROR("GetNextLine() error");
                        exit(-1);
                    }
                }

                line = token + line;
                // skip function
                std::regex regRaw(R"((virtual\s+)?(\w+)?(\s+)?~?(\w+(?:=|==)?)?\((.*)\)(\s+const)?(.*))");
                if (std::regex_match(token + line, regRaw)) {
                    ParseFunctionSkip(src, pos);
                    continue;
                }

                if (!ParseDeclaration(line, member)) {
                    SPDLOG_ERROR("Unresolved struct/union member declaration syntax");
                    return false;
                }

                // parse comments
                size_t index = pos;
                GetNextToken(src, index, next_token);
                if (next_token == "/") {
                    ParseComment(src, pos, member.m_comment);
                } else {
                    member.m_comment = "";
                }

                spdlog::info("Add member: " + member.m_name);
                members.push_back(member);
            }
        }
    }

    return true;
}

// get rest part of the line
bool TypeParser::GetRestLine(const std::string& src, size_t& pos, std::string& line) const
{
    if (EOL == src[pos]) {
        line.clear();
        return false;
    }

    size_t p = src.find(EOL, pos);
    if (std::string::npos != p) {
        line = src.substr(pos, p - pos);
        pos  = p;
    } else {
        line = src.substr(pos);
        pos  = src.length();
    }

    return true;
}

/// Parsing enum member declaration
///
/// Possible formats:
///		1) Zhejiang             // only for last enum member
///		2) Beijing,
///		3) Shenzhen = <value>   // only for last enum member
///		4) Shanghai = <value>,
///
/// @param[in]		line		the declaration of a enum member
/// @param[in,out]	last_value	[in]the value of last enum member; [out]the value of current enum member
/// @param[out]		decl		enum member declaration
/// @param[out]		is_last_member	true for format 2 & 4, else false
bool TypeParser::ParseEnumDeclaration(const std::string& line,
                                      int&               last_value,
                                      EnumDeclaration&   decl,
                                      bool&              is_last_member) const
{
    // whether this enum variable is the lastest member of the enum type
    is_last_member = false;
    std::vector<std::string> tokens;
    long                     number;

    switch (SplitLineIntoTokens(line, tokens)) {
    case 1:
        is_last_member = true;
        decl.m_value   = ++last_value;
        break;

    case 2:
        if (kComma != tokens[1].at(0)) {
            SPDLOG_ERROR("token[1] is not kComma");
            exit(-1);
        }
        decl.m_value = ++last_value;
        break;

    case 3:
        if (kEqual != tokens[1].at(0)) {
            SPDLOG_ERROR("token[1] is not kEqual");
            exit(-1);
        }

        if (!IsNumericToken(tokens[2], number)) {
            SPDLOG_ERROR("Cannot convert token into a number - " + tokens[2]);
            return false;
        }

        is_last_member = true;
        decl.m_value = last_value = static_cast<int>(number);
        break;

    case 4:
        if (kEqual != tokens[1].at(0) || kComma != tokens[3].at(0)) {
            SPDLOG_ERROR("token[1] is not kEqual, token[3] is not kEqual");
            exit(-1);
        }

        if (!IsNumericToken(tokens[2], number)) {
            SPDLOG_ERROR("Cannot convert token into a number - " + tokens[2]);
            return false;
        }

        decl.m_value = last_value = static_cast<int>(number);
        break;

    default:
        SPDLOG_ERROR("Bad syntax for enum member declaration - " + line);
        return false;
    }

    decl.m_name = tokens[0];
    return true;
}

/// Parse a variable declaration
///
/// A declaration can be as complicated as:
///     unsigned char *array[MAX_SIZE]; // qualifiers should be removed from "line" argument
///     struct <complex_type> var;      // the struct/union/enum keyword should be removed from "line" argument
/// @note code lines with multiple variables declared consecutively are ignored, like "int a, b, c = MAX;"
///
/// @param[in]  line    a code line that ends with kSemicolon and is stripped of preceding qualifiers (like unsigned)
///                     and struct/union/enum keywords
/// @param[out] decl    the variable declaration if the line is parsed successfully
/// @return             true if the line can be parsed into a variable declaration successfully
///
/// @note type size are calculated will simple consideration of alignment
/// @note can be improved with consideration of multiple demension array
bool TypeParser::ParseDeclaration(const std::string& line, StructDeclaration& decl) const
{
    if (line.empty()) {
        SPDLOG_ERROR("line is empty");
        exit(-1);
    }
    if (line[line.length() - 1] != kSemicolon)
        return false;

    std::vector<std::string> tokens;
    size_t                   size = SplitLineIntoTokens(line, tokens);
    // even the simplest declaration contains 3 tokens: type var ;
    if (size < 3) {
        SPDLOG_ERROR("size less 3");
        exit(-1);
    }

    size_t index = 0;
    // multiple declarations
    decl.m_type = tokens[index];
    while (kBasicDataType == GetTokenType(tokens[++index])) {
        decl.m_type += " " + tokens[index];
    }
    --index;
    decl.m_is_pointer = false;

    size_t length = GetTypeSize(decl.m_type);
    if (0 == length) {
        spdlog::debug("Unknown data type - " + decl.m_type);
        return false;
    }

    if (tokens[++index].at(0) == kAsterisk) {
        decl.m_is_pointer = true;
        length            = kWordSize_; // size of a pointer is 4 types on a 32-bit system
        decl.m_name       = tokens[++index];
    } else {
        decl.m_name = tokens[index];
    }

    if (tokens[++index].at(0) == '[') {
        long number;
        if (IsNumericToken(tokens[++index], number)) {
            decl.m_size_array = number;
            length *= number;
        } else {
            SPDLOG_ERROR("Array size cannot be parsed into a number - " + tokens[index]);
            return false;
        }
    } else {
        decl.m_size_array = 0;
    }

    decl.m_size_value = length;

    return true;
}

/// Parse assignment expression
///
/// @param[in]  line    an assignment expression with the format: var = number
/// @return             true if the line can be parsed successfully, and @var const_defs_ will be updated
///
bool TypeParser::ParseAssignExpression(const std::string& line)
{
    std::vector<std::string> tokens;
    long                     number;

    // only 4 tokens for an assignment expression: var = number;
    if (4 == SplitLineIntoTokens(line, tokens) && kEqual == tokens[1].at(tokens[1].length() - 1) && kSemicolon == tokens[3].at(tokens[3].length() - 1) && IsNumericToken(tokens[2], number)) {
        const_defs_.insert(make_pair(tokens[0], number));
        return true;
    }

    return false;
}

/// split a line into tokens
///
/// @note: keywords that can be ignored will be removed by GetNextToken()
size_t TypeParser::SplitLineIntoTokens(std::string line, std::vector<std::string>& tokens) const
{
    std::string token;
    size_t      start = 0;

    while (GetNextToken(line, start, token)) {
        tokens.push_back(token);
    }

    return tokens.size();
}

/// Pad a struct with padding fields for memory alignment
///
/// @param[in,out] members  struct members, will be inserted with padding fields when needed
/// @return					struct size after alignment
///
/// @note This method is based on kAlignment_ = 4 on 32-bit system since the padding algorithm can be very complicated
/// considering the different alignment modulus/options of different compiler/OS/CPU
/// About alignment, @see http://c-faq.com/struct/align.esr.html
size_t TypeParser::PadStructMembers(std::list<StructDeclaration>& members)
{
    auto   it    = members.begin();
    size_t total = 0;
    size_t size;
    size_t last_size = 0; ///< when last_size>0, padding is needed depending on later members; else no more padding is needed later
    size_t align_size, pad_size;

    while (it != members.end()) {
        size = it->m_size_value;

        if (0 == (size % kAlignment_)) { // current member itself is aligned
            if (last_size > 0) {         // need padding previous members to alignment
                align_size = static_cast<size_t>(ceil(last_size * 1.0 / kAlignment_) * kAlignment_);
                pad_size   = align_size - last_size;
                members.insert(it, MakePadField(pad_size));

                total += align_size;
            } else {
                total += size;
            }

            last_size = 0;
            ++it;
        } else { // current member itself cannot align
            // size can only be less than 4 (1 or 2) now unless it's an array
            if (size >= kAlignment_) {
                if (it->m_size_array > 0) {
                    spdlog::debug("TODO: add array support in PadStructMembers()");
                } else {
                    SPDLOG_ERROR("Incorrect type size for " + it->m_name);
                }

                return 0;
            }

            if (0 == last_size) { // last member is aligned
                last_size = size;
                ++it;
            } else if (0 == (size + last_size) % kAlignment_) {
                total += size + last_size;
                last_size = 0;
                ++it;
            } else if (1 == last_size) {
                if (1 == size) {
                    last_size += 1;
                    ++it;
                } else if (2 == size) {
                    members.insert(it, MakePadField(1));
                    total += kAlignment_;
                    last_size = 0;
                    ++it;
                } else {
                    SPDLOG_ERROR("Bad member size - " + std::to_string(size));
                    return 0;
                }
            } else if (2 == last_size) {
                if (1 != size) {
                    SPDLOG_ERROR("size not equal 1");
                    exit(-1);
                }
                members.insert(++it, MakePadField(1));
                total += kAlignment_;
                last_size = 0;
            } else {
                SPDLOG_ERROR("Bad alignment");
                return 0;
            }
        }
    }

    return total;
}

size_t TypeParser::CalcUnionSize(const std::list<StructDeclaration>& members) const
{
    size_t size = 0;
    for (std::list<StructDeclaration>::const_iterator it = members.begin(); it != members.end(); ++it) {
        size = std::max(size, it->m_size_value);
    }

    if (0 != size % kAlignment_) {
        size = static_cast<size_t>(std::ceil(size * 1.0 / kAlignment_) * kAlignment_);
    }

    return size;
}

StructDeclaration TypeParser::MakePadField(const size_t size) const
{
    StructDeclaration var;

    var.m_name       = kPaddingFieldName;
    var.m_size_value = size;
    var.m_type       = "char";
    var.m_size_array = 0;
    var.m_is_pointer = false;

    return var;
}

/// Store the definition and size of a struct or union
///
/// For structs, the members are padded based on alignment, @see TypeParser::PadStructMembers
///
void TypeParser::StoreStructUnionDef(const bool is_struct, const std::string& type_name, std::list<StructDeclaration>& members)
{
    size_t size;

    if (is_struct) {
        size                    = PadStructMembers(members);
        struct_defs_[type_name] = members;
    } else {
        size                   = CalcUnionSize(members);
        union_defs_[type_name] = members;
    }

    type_sizes_[type_name] = size;
}

bool TypeParser::ParseComment(const std::string& src, size_t& pos, std::string& comment) const
{
    size_t      index = pos;
    std::string token;
    std::string line;

    GetNextToken(src, index, token);
    if (token == "/") {
        GetRestLine(src, index, line);
        comment = token + line;
        if (comment[0] == '/' && comment[1] == '/') {
            comment = comment.substr(2, comment.size() - 2);
            comment = trim(comment);
            pos     = index;
            return true;
        } else if (comment[0] == '/' && comment[1] == '*') {
            while (true) {
                if ('*' == comment[comment.size() - 2] && '/' == comment[comment.size() - 1]) {
                    comment = comment.substr(2, comment.size() - 4);
                    comment = trim(comment);
                    pos     = index;
                    return true;
                }
                GetNextToken(src, index, token);
                GetRestLine(src, index, line);
                comment += token + line;
            }
        }
    }
    return false;
}

bool TypeParser::ParseCommentSkip(const std::string& src, size_t& pos) const
{
    size_t      index = pos;
    std::string token;
    std::string line;

    GetNextToken(src, index, token);
    if (token == "/") {
        GetRestLine(src, index, token);
        pos = index;
        return true;
    } else if (token == "*") {
        GetRestLine(src, index, token);
        while (true) {
            if (token[token.size() - 2] == '*' && token[token.size() - 1] == '/') {
                pos = index;
                return true;
            }
            GetNextLine(src, index, token);
        }
    }
    return false;
}

bool TypeParser::ParseFunctionSkip(const std::string& src, size_t& pos) const
{
    size_t      index = pos, num = 0;
    std::string token;
    std::string line;

    GetNextToken(src, index, token);
    while (true) {
        if (kBlockStart == token.at(0)) {
            num++;
        } else if (kBlockEnd == token.at(0)) {
            num--;
        }
        if (kBlockEnd == token.at(0) && num == 0) {
            pos = index;
            return true;
        }
        GetNextToken(src, index, token);
    }
    return false;
}

std::string TypeParser::GetNamespace(size_t& pos) const
{
    std::string space;
    for (auto it = namespaces_.begin(); it != namespaces_.end(); ++it) {
        if (std::get<1>(*it) < pos && std::get<2>(*it) == 0) {
            space += std::get<0>(*it);
            space += "::";
        }
    }
    return space;
}
