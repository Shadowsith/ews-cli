#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <exception>
#include <unordered_map>
#include <optional>
#include <ews/ews.hpp>
#include <nlohmann/json.hpp>

class EwsHandler
{
public:
    static void init(const std::vector<std::string> &args);
    static ews::service get_service();
    static ews::mailbox get_mailbox();
    static ews::standard_folder get_standard_folder();
    static ews::distinguished_folder_id get_standard_folder_id();
    static void handle_action(ews::service &service);
    static std::string get_folder_full_path(ews::service &service, ews::folder_id folder_id);
    static ews::folder find_folder(ews::service &service);
    static ews::folder find_folder(ews::service &service, ews::folder_id &folder);
    static ews::folder find_folder(ews::service &service, std::string &folder_path);
    static std::vector<ews::folder> find_folders(ews::service &service, std::string &folder_path);
    static ews::folder create_folder(ews::service &service, std::string &folder_path);
    static void get_mails(ews::service &service);
    static void get_mails(ews::service &service, std::string &folder_path);
    // static bool delete_folder(ews::service &service, std::string folder_path);

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
    static std::string _folder_path;
    static std::string _containment_mode;

    static ews::containment_mode get_containment_mode();
    static ews::contains get_search_expression();
    static std::vector<std::string> split(const std::string &s, char delimiter);
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
std::string EwsHandler::_folder_path;
std::string EwsHandler::_containment_mode;

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
        {"--folder-path", &EwsHandler::_folder_path},
        {"--containment-mode", &EwsHandler::_containment_mode},
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

ews::distinguished_folder_id EwsHandler::get_standard_folder_id()
{
    std::string mb = EwsHandler::_mailbox;
    auto standard_folder = EwsHandler::get_standard_folder();

    if (mb != "")
    {
        // search mailbox
        auto mailbox = EwsHandler::get_mailbox();
        return ews::distinguished_folder_id(standard_folder, mailbox);
    }
    else
    {
        // search main account
        return ews::distinguished_folder_id(standard_folder);
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

ews::folder EwsHandler::find_folder(ews::service &service, std::string &folder_path)
{
    // Split the folder_path by '/'
    std::vector<std::string> folder_names = EwsHandler::split(folder_path, '/');
    ews::distinguished_folder_id standard_folder_id = EwsHandler::get_standard_folder_id();

    // Ensure the root folder is "inbox" if no root is specified
    if (folder_names.empty())
    {
        throw ews::exception("--folder-path parameter empty");
    }
    else if (folder_names[0].empty())
    {
        throw ews::exception("--folder-path parameter empty");
    }

    std::optional<ews::folder_id> parent_folder_id;

    for (const auto &folder_name : folder_names)
    {
        if (!parent_folder_id)
        {
            auto parent_folder = service.get_folder(standard_folder_id);
            parent_folder_id = parent_folder.get_folder_id();
        }

        std::optional<ews::folder_id> folder_id;
        try
        {
            // Check if the folder already exists
            auto folder_ids = service.find_folder(parent_folder_id.value());
            for (const auto &id : folder_ids)
            {
                ews::folder f = service.get_folder(id);
                if (f.get_display_name() == folder_name)
                {
                    folder_id = id;
                }
            }
            if (!folder_id)
            {
                throw ews::exception("Folder not found");
            }
        }
        catch (ews::exception &e)
        {
            // Failed to create folder, throw exception
            std::string error_msg = "Failed to find folder: " + folder_name;
            throw std::runtime_error(error_msg + ", " + e.what());
        }
        // Update the parent folder ID for the next iteration
        parent_folder_id = folder_id.value();
    }
    return service.get_folder(parent_folder_id.value());
}

std::vector<ews::folder> EwsHandler::find_folders(ews::service &service, std::string &folder_path)
{
    ews::folder parent_folder = EwsHandler::find_folder(service, folder_path);
    std::vector<ews::folder> folders;
    auto folder_ids = service.find_folder(parent_folder.get_folder_id());
    for (const auto &id : folder_ids)
    {
        ews::folder f = service.get_folder(id);
        folders.push_back(f);
    }
    return folders;
}

ews::folder EwsHandler::create_folder(ews::service &service, std::string &folder_path)
{
    // Split the folder_path by '/'
    std::vector<std::string> folder_names = EwsHandler::split(folder_path, '/');
    ews::distinguished_folder_id standard_folder_id = EwsHandler::get_standard_folder_id();

    // Ensure the root folder is "inbox" if no root is specified
    if (folder_names.empty())
    {
        throw ews::exception("--folder-path parameter empty");
    }
    else if (folder_names[0].empty())
    {
        throw ews::exception("--folder-path parameter empty");
    }

    std::optional<ews::folder_id> parent_folder_id;

    for (const auto &folder_name : folder_names)
    {
        if (!parent_folder_id)
        {
            auto parent_folder = service.get_folder(standard_folder_id);
            parent_folder_id = parent_folder.get_folder_id();
        }

        std::optional<ews::folder_id> folder_id;
        try
        {
            // Check if the folder already exists
            auto folder_ids = service.find_folder(parent_folder_id.value());
            for (const auto &id : folder_ids)
            {
                ews::folder f = service.get_folder(id);
                if (f.get_display_name() == folder_name)
                {
                    folder_id = id;
                }
            }
            if (!folder_id)
            {
                throw ews::exception("Folder not found");
            }
        }
        catch (ews::exception &e)
        {
            // Folder does not exist, try creating it
            try
            {
                auto new_folder_id = ews::folder_id();
                auto new_folder = ews::folder(new_folder_id);
                new_folder.set_display_name(folder_name);

                folder_id = service.create_folder(new_folder, parent_folder_id.value());
            }
            catch (const ews::exception &e)
            {
                // Failed to create folder, throw exception
                std::string error_msg = "Failed to create folder: " + folder_name;
                throw std::runtime_error(error_msg + ", " + e.what());
            }
        }
        // Update the parent folder ID for the next iteration
        parent_folder_id = folder_id.value();
    }

    return service.get_folder(parent_folder_id.value());
}

void EwsHandler::get_mails(ews::service &service)
{
    auto standard_folder_id = EwsHandler::get_standard_folder_id();
    auto item_ids = service.find_item(standard_folder_id);
    std::vector<EwsMailData> list = std::vector<EwsMailData>();
    nlohmann::json j;

    for (const auto &id : item_ids)
    {
        auto msg = service.get_message(id);
        auto data = EwsMailData(msg);
        j += data.to_json();
    }
    std::cout << j.dump(4) << std::endl;
}

void EwsHandler::get_mails(ews::service &service, std::string &folder_path)
{
    auto folder = EwsHandler::find_folder(service, folder_path);
    auto item_ids = service.find_item(folder.get_folder_id());
    std::vector<EwsMailData> list = std::vector<EwsMailData>();

    for (const auto &id : item_ids)
    {
        auto msg = service.get_message(id);
        auto data = EwsMailData(msg);
        list.push_back(data);
        auto j = data.to_json();
        std::cout << j.dump(4) << std::endl;
    }
}

/*
static bool delete_folder(ews::service &service, std::string folder_path)
{
    // Split the folder_path by '/'
    std::vector<std::string> folders = EwsHandler::split(folder_path, '/');

    // Ensure the root folder is "inbox" if no root is specified
    if (folders.empty())
    {
        throw std::invalid_argument("Folder path cannot be empty");
    }
    else if (folders[0].empty())
    {
        folders[0] = "inbox";
    }

    ews::folder_id parent_folder_id = ews::distinguished_folder_id(EwsHandler::get_standard_folder());
    ews::folder_id target_folder_id;
    for (const auto &folder_name : folders)
    {
        ews::folder_id folder_id;
        try
        {
            // Try to find the folder
            folder_id = service.get_folder(parent_folder_id, folder_name).id();
        }
        catch (ews::exception &)
        {
            // Folder does not exist, throw exception
            throw std::runtime_error("Folder not found: " + folder_name);
        }
        // Update the parent folder ID for the next iteration
        parent_folder_id = folder_id;
        target_folder_id = folder_id; // Keep track of the current folder ID
    }

    try
    {
        // Try to delete the target folder
        service.delete_folder(target_folder_id);
        return true;
    }
    catch (const ews::exception &)
    {
        // Failed to delete the folder
        return false;
    }
}
*/

void EwsHandler::handle_action(ews::service &service)
{
    std::string a = EwsHandler::_action;

    if (a == "create_folder")
    {
        auto folder = EwsHandler::create_folder(service, EwsHandler::_folder_path);
        std::cout << EwsFolderData(folder, EwsHandler::_folder_path).to_json().dump() << std::endl;
    }
    else if (a == "find_folder")
    {
        auto folder = EwsHandler::find_folder(service, EwsHandler::_folder_path);
        std::cout << EwsFolderData(folder, EwsHandler::_folder_path).to_json().dump() << std::endl;
    }
    else if (a == "find_folders")
    {
        std::vector<ews::folder> folders = EwsHandler::find_folders(service, EwsHandler::_folder_path);
        nlohmann::json j;
        for (const auto &f : folders)
        {
            j += EwsFolderData(f).to_json();
        }
        std::cout << j.dump() << std::endl;
    }
    else if (a == "delete_folder")
    {
        std::string path = EwsHandler::_folder_path;
    }
    else if (a == "move_folder")
    {
    }
    else if (a == "get_mails")
    {
        if (EwsHandler::_folder_path == "")
        {
            EwsHandler::get_mails(service);
        }
        else
        {
            EwsHandler::get_mails(service, EwsHandler::_folder_path);
        }
    }
    else
    {
        throw std::runtime_error("No valid action");
    }
}

// private
ews::containment_mode EwsHandler::get_containment_mode()
{
    auto mode = EwsHandler::_containment_mode;
    if (mode == "exact_phrase")
    {
        return ews::containment_mode::exact_phrase;
    }
    else if (mode == "full_string")
    {
        return ews::containment_mode::full_string;
    }
    else if (mode == "prefix_on_words")
    {
        return ews::containment_mode::prefix_on_words;
    }
    else if (mode == "prefixed")
    {
        return ews::containment_mode::prefixed;
    }
    else if (mode == "")
    {
        return ews::containment_mode::substring;
    }
    else
    {
        return ews::containment_mode::substring;
    }
}

ews::contains EwsHandler::get_search_expression()
{
    return ews::contains(ews::item_property_path::subject, "ess",
                         ews::containment_mode::substring, ews::containment_comparison::ignore_case);
}

std::vector<std::string> EwsHandler::split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}