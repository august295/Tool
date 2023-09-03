#include <fstream>
#include <json/json.h>

#include "Config.h"
#include "Log.h"

Config::Config(const char* file)
    : mFile(file)
{
    std::ifstream ifs(mFile, std::ios::binary);

    if (!ifs.is_open())
    {
        LOGE("open %s error", file);
        return;
    }
    else
    {
        Json::CharReaderBuilder builder;
        builder["collectComments"] = true;
        JSONCPP_STRING errs;
        Json::Value    root;

        if (parseFromStream(builder, ifs, &root, &errs))
        {
            this->ip                     = root["ip"].asString();
            this->port                   = root["port"].asInt();
            this->threadNum              = root["threadNum"].asInt();
            this->m_video_path           = root["video"]["video_path"].asString();
            Json::Value video_name_array = root["video"]["video_name"];
            for (auto iter = video_name_array.begin(); iter != video_name_array.end(); ++iter)
            {
                this->m_video_name_vec.emplace_back(iter->asString());
            }

            state = true;
        }
        else
        {
            LOGE("parse %s error", file);
        }
        ifs.close();
    }
}

Config::~Config()
{
}

const char* Config::getIp() const
{
    return ip.data();
}

int Config::getPort() const
{
    return port;
}

int Config::getThreadNum() const
{
    return threadNum;
}

std::string Config::GetVideoPath() const
{
    return m_video_path;
}

std::vector<std::string> Config::GetVideoNameVec() const
{
    return m_video_name_vec;
}
