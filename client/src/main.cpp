#include <print>
#include "client_application/ClientApplication.h"

int main()
{
    try
    {
        sLink::client_application::ClientApplication application(800, 600, "Client");

        application.run();
    } catch (const std::exception &exception)
    {
        std::println(stderr, "CLIENT ERROR: {}", exception.what());
    }
}