#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>

class Config
{
public:
    Config(const char* file);
    ~Config();

public:
    bool state = false;

    const char* getIp() const;
    int         getPort() const;
    int         getThreadNum() const;

    std::string              GetVideoPath() const;
    std::vector<std::string> GetVideoNameVec() const;

private:
    std::string              mFile;            // config file
    std::string              ip;               // the connect IP
    int                      port;             // the connect port
    int                      threadNum;        // the thread num
    std::string              m_video_path;     // the video storage path
    std::vector<std::string> m_video_name_vec; // the vidoe name
};

#endif // CONFIG_H
