#include <iostream>
#include <string>
#include <unordered_map>
#include <ews/ews.hpp>
#include <nlohmann/json.hpp>

class EwsHandler
{
public:
    static void init(const std::vector<std::string> &args);
    static ews::service get_service();
    static ews::mailbox get_mailbox();
    static ews::standard_folder get_standard_folder();
    static void handle_action(ews::service& service);
    static void search_folder(ews::service& service);

private:
    static std::string _server_uri;
    static std::string _domain;
    static std::string _username;
    static std::string _password;
    static std::string _action;
    static std::string _option;
    static std::string _mailbox;
    static std::string _folder;
};

std::string EwsHandler::_server_uri;
std::string EwsHandler::_domain;
std::string EwsHandler::_username;
std::string EwsHandler::_password;
std::string EwsHandler::_action;
std::string EwsHandler::_option;
std::string EwsHandler::_mailbox;
std::string EwsHandler::_folder;

void EwsHandler::init(const std::vector<std::string> &args)
{
    // Eine map mit den Parametern und den zugehörigen Variablen
    std::unordered_map<std::string, std::string *> paramMap = {
        {"--url", &EwsHandler::_server_uri},
        {"--domain", &EwsHandler::_domain},
        {"--user", &EwsHandler::_username},
        {"--password", &EwsHandler::_password},
        {"--mailbox", &EwsHandler::_mailbox},
        {"--folder", &EwsHandler::_folder},
        {"--option", &EwsHandler::_option},
        {"--action", &EwsHandler::_action}};

    for (size_t i = 0; i < args.size(); i++)
    {
        // Wenn das aktuelle Argument in paramMap ist und es gibt ein nächstes Argument
        if (paramMap.find(args[i]) != paramMap.end() && i + 1 < args.size())
        {
            *paramMap[args[i]] = args[i + 1]; // Weise der zugehörigen Variable den Wert des nächsten Arguments zu
            i++;                              // Überspringe den Wert
        }
    }

    /*
    std::cout << "URL: " << EwsHandler::_server_uri << std::endl;
    std::cout << "D: " << EwsHandler::_domain << std::endl;
    std::cout << "U: " << EwsHandler::_username << std::endl;
    std::cout << "PW: " << EwsHandler::_password << std::endl;
    std::cout << "Action: " << EwsHandler::_action << std::endl;
    std::cout << "Option: " << EwsHandler::_option << std::endl;
    std::cout << "Folder: " << EwsHandler::_folder << std::endl;
    std::cout << "Mailbox: " << EwsHandler::_mailbox << std::endl;
    */
}

ews::service EwsHandler::get_service()
{
    return ews::service(
        EwsHandler::_server_uri,
        EwsHandler::_domain,
        EwsHandler::_username,
        EwsHandler::_password);
}

ews::mailbox EwsHandler::get_mailbox()
{
    return ews::mailbox(EwsHandler::_mailbox);
}

ews::standard_folder EwsHandler::get_standard_folder()
{
    std::string f = EwsHandler::_folder;
    if (f == "inbox")
    {
        return ews::standard_folder::inbox;
    }
    else if (f == "sent_items")
    {
        return ews::standard_folder::sent_items;
    }
    else if (f == "drafts")
    {
        return ews::standard_folder::drafts;
    }
    else if (f == "junk_mail" || f == "junk")
    {
        return ews::standard_folder::junk_email;
    }
    else
    {
        return ews::standard_folder::inbox;
    }
}

void EwsHandler::search_folder(ews::service& service)
{
    std::string mb = EwsHandler::_mailbox;
    std::string folder = EwsHandler::_folder;
    ews::distinguished_folder_id folder_id;


    if (mb != "") {
        // serach mailbox
        auto mailbox = EwsHandler::get_mailbox();
        auto folder = EwsHandler::get_standard_folder();
        folder_id = ews::distinguished_folder_id(folder, mailbox);
    }
    else {
        // search main account
    }

    auto item_ids = service.find_item(folder_id);
    std::vector<EwsMailData> list = std::vector<EwsMailData>();

    for (const auto& id : item_ids) {
        auto msg = service.get_message(id);
        auto data = EwsMailData(msg);
        list.push_back(data);
        /*
        auto j = data.to_json();
        std::cout << j.dump(4) << std::endl;
        */
    }

}

void EwsHandler::handle_action(ews::service& service)
{
    std::string a = EwsHandler::_action;
    std::string o = EwsHandler::_option;

    if (a == "create_folder")
    {
    }
    else if (a == "delete_folder")
    {
    }
    else if (a == "move_folder")
    {
    }
    else if (a == "search_folder")
    {
        EwsHandler::search_folder(service);
    }
    else
    {
    }
}