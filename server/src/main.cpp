#include <print>
#include <application/Application.h>

int main()
{
    try
    {
        sLink::application::Application application(800, 600, "Server");

        application.run();
    } catch (const std::exception &exception)
    {
        std::println(stderr, "SERVER ERROR: {}", exception.what());
    }
}
