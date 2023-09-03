#include "Scheduler.h"
#include "Server/BoostServer.h"
#include "Utils/Config.h"

int main(int argc, char* argv[])
{
    const char* file = NULL;
    file             = "../../resources/config.json";

    Config config(file);
    if (!config.state)
        {
            printf("failed to read config file: %s\n", file);
            return -1;
        }

    BoostServer server(&config);
    std::thread([&]() {
        server.start();
    }).detach();

    Scheduler sch(&server, &config);
    sch.loop();

    return 0;
}
