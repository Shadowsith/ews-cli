#include <exception>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include <ews/ews.hpp>

#include "lib/EwsMailData.hpp"
#include "lib/EwsHandler.hpp"

int main(int argc, char *argv[])
{
    std::vector<std::string> args(argv, argv + argc);
    ews::set_up();

    try
    {
        EwsHandler::init(args);
        auto service = EwsHandler::get_service();
        EwsHandler::handle_action(service);
    }
    catch (std::exception &exc)
    {
        std::cout << exc.what() << std::endl;
    }

    ews::tear_down();
    return 0;
}
