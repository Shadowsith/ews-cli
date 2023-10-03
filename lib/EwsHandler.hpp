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
    static void handle_action(ews::service &service);
    static std::string get_folder_full_path(ews::service &service, ews::folder_id folder_id);
    static ews::folder find_folder(ews::service &service);
    static ews::folder find_folder(ews::service &service, ews::folder_id &folder);
    static void search_folder(ews::service &service);

private:
    static std::string _server_uri;
    static std::string _domain;
    static std::string _username;
    static std::string _password;
    static std::string _action;
    static std::string _option;
    static std::string _mailbox;
    static std::string _folder;
    static std::string _standard_folder;
    static std::string _parent_folder;
};

std::string EwsHandler::_server_uri;
std::string EwsHandler::_domain;
std::string EwsHandler::_username;
std::string EwsHandler::_password;
std::string EwsHandler::_action;
std::string EwsHandler::_option;
std::string EwsHandler::_mailbox;
std::string EwsHandler::_folder;
std::string EwsHandler::_standard_folder;
std::string EwsHandler::_parent_folder;

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
        {"--standard-folder", &EwsHandler::_standard_folder},
        {"--parent-folder", &EwsHandler::_parent_folder},
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
    std::string f = EwsHandler::_standard_folder;
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

std::string EwsHandler::get_folder_full_path(ews::service &service, ews::folder_id folder_id)
{
    ews::distinguished_folder_id start_folder = EwsHandler::get_standard_folder();
    auto f = service.get_folder(folder_id);
    std::string full_path = f.get_display_name();
    try
    {
        auto sf = service.get_folder(start_folder);
        auto pf = service.get_folder(f.get_parent_folder_id());
        if (sf.get_display_name() == pf.get_display_name())
        {
            full_path = sf.get_display_name() + "<<" + full_path;
        }
        else
        {
            std::string parent_name = get_folder_full_path(service, f.get_parent_folder_id());
            full_path = get_folder_full_path(service, f.get_parent_folder_id()) + "<<" + full_path;
        }
    }
    catch (std::exception &exc)
    {
    }
    return full_path;
}

ews::folder EwsHandler::find_folder(ews::service &service)
{
    ews::distinguished_folder_id parent_folder = EwsHandler::get_standard_folder();
    auto sub_folder_ids = service.find_folder(parent_folder);
    std::string folder_name = EwsHandler::_folder;
    ews::folder folder;

    // Now iterate over all sub folder and display their ID
    for (auto &folder_id : sub_folder_ids)
    {
        auto f = service.get_folder(folder_id);
        if (f.get_display_name() == folder_name)
        {
            std::cout << f.get_display_name() << std::endl;
            folder = f;
            break;
        }
        if (f.get_child_folder_count() > 0)
        {
            folder = EwsHandler::find_folder(service, folder_id);
            try
            {
                folder.get_folder_id();
            }
            catch (std::exception &exc)
            {
                continue;
            }
        }
    }
    return folder;
}

ews::folder EwsHandler::find_folder(ews::service &service, ews::folder_id &parent_folder)
{
    auto sub_folder_ids = service.find_folder(parent_folder);
    std::string folder_name = EwsHandler::_folder;
    ews::folder folder;

    // Now iterate over all sub folder and display their ID
    for (auto &folder_id : sub_folder_ids)
    {
        auto f = service.get_folder(folder_id);
        auto pf = service.get_folder(parent_folder);
        if (f.get_display_name() == folder_name)
        {
            std::cout << EwsHandler::get_folder_full_path(service, folder_id) << std::endl;
            folder = f;
            break;
        }
        if (f.get_child_folder_count() > 0)
        {
            folder = EwsHandler::find_folder(service, folder_id);
            try
            {
                folder.get_folder_id();
            }
            catch (std::exception &exc)
            {
                continue;
            }
        }
    }
    return folder;
}

void EwsHandler::search_folder(ews::service &service)
{
    std::string mb = EwsHandler::_mailbox;
    std::string folder = EwsHandler::_folder;
    ews::distinguished_folder_id folder_id;
    auto standard_folder = EwsHandler::get_standard_folder();

    if (mb != "")
    {
        // search mailbox
        auto mailbox = EwsHandler::get_mailbox();
        folder_id = ews::distinguished_folder_id(standard_folder, mailbox);
    }
    else
    {
        // search main account
        folder_id = ews::distinguished_folder_id(standard_folder);
    }

    auto item_ids = service.find_item(folder_id);
    std::vector<EwsMailData> list = std::vector<EwsMailData>();

    for (const auto &id : item_ids)
    {
        auto msg = service.get_message(id);
        auto data = EwsMailData(msg);
        list.push_back(data);
        /*
        auto j = data.to_json();
        std::cout << j.dump(4) << std::endl;
        */
    }
}

void EwsHandler::handle_action(ews::service &service)
{
    std::string a = EwsHandler::_action;
    std::string o = EwsHandler::_option;

    if (a == "create_folder")
    {
    }
    else if (a == "find_folder")
    {
        EwsHandler::find_folder(service);
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