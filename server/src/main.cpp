#include <print>
#include "server_application/ServerApplication.h"

int main()
{
    try
    {
        sLink::server_application::ServerApplication application(800, 600, "Server");

        application.run();
    } catch (const std::exception &exception)
    {
        std::println(stderr, "SERVER ERROR: {}", exception.what());
    }
}
