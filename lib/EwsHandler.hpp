#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <exception>
#include <unordered_map>
#include <optional>
#include <variant>
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
    static void search_mails(ews::service &service);
    static void search_mails(ews::service &service, std::string &folder_path);
    static void move_mail(ews::service &service, std::string &item_id, std::string &change_key, std::string &folder_path, std::string &dest_folder_path);
    static void move_mails(ews::service &service, std::string &folder_path, std::string &dest_folder_path);
    // static bool delete_folder(ews::service &service, std::string folder_path);
    static void show_help();
    static void show_version();

private:
    static bool _show_help;
    static bool _show_version;
    static std::string _version;
    static std::string _server_uri;
    static std::string _domain;
    static std::string _username;
    static std::string _password;
    static std::string _action;
    static std::string _mailbox;
    static std::string _folder;
    static std::string _folder_id;
    static std::string _item_id;
    static std::string _change_key;
    static std::string _standard_folder;
    static std::string _folder_path;
    static std::string _dest_folder_path;
    static std::string _containment_mode;
    static std::string _containment_comparison;
    static std::string _item_property_path;
    static std::string _search_type;
    static std::string _search_filter;
    static std::string _search_filter_type;

    static ews::containment_mode get_containment_mode();
    static ews::containment_comparison get_containment_comparison();
    static ews::property_path get_item_property_path();
    static ews::search_expression get_search_expression();
    static ews::is_equal_to get_is_equal_to(ews::property_path property_path, const std::string &value, std::string type);
    static ews::is_not_equal_to get_is_not_equal_to(ews::property_path property_path, const std::string &value, std::string type);
    static ews::is_greater_than get_is_greater_than(ews::property_path property_path, const std::string &value, std::string type);
    static ews::is_greater_than_or_equal_to get_is_greater_than_or_equal_to(ews::property_path property_path, const std::string &value, std::string type);
    static ews::is_less_than get_is_less_than(ews::property_path property_path, const std::string &value, std::string type);
    static ews::is_less_than_or_equal_to get_is_less_than_or_equal_to(ews::property_path property_path, const std::string &value, std::string type);
    static std::vector<std::string> split(const std::string &s, char delimiter);
    static std::variant<int, bool, const char *, ews::date_time> type_cast(const std::string &value, std::string type);
};

bool EwsHandler::_show_help;
bool EwsHandler::_show_version;
std::string EwsHandler::_version = "0.0.1";
std::string EwsHandler::_server_uri;
std::string EwsHandler::_domain;
std::string EwsHandler::_username;
std::string EwsHandler::_password;
std::string EwsHandler::_action;
std::string EwsHandler::_mailbox;
std::string EwsHandler::_folder;
std::string EwsHandler::_folder_id;
std::string EwsHandler::_item_id;
std::string EwsHandler::_change_key;
std::string EwsHandler::_standard_folder;
std::string EwsHandler::_folder_path;
std::string EwsHandler::_dest_folder_path;
std::string EwsHandler::_containment_mode;
std::string EwsHandler::_containment_comparison;
std::string EwsHandler::_item_property_path;
std::string EwsHandler::_search_type;
std::string EwsHandler::_search_filter;
std::string EwsHandler::_search_filter_type;

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
        {"--folder-id", &EwsHandler::_folder_id},
        {"--item-id", &EwsHandler::_item_id},
        {"--change-key", &EwsHandler::_change_key},
        {"--standard-folder", &EwsHandler::_standard_folder},
        {"--folder-path", &EwsHandler::_folder_path},
        {"--dest-folder-path", &EwsHandler::_dest_folder_path},
        {"--containment-mode", &EwsHandler::_containment_mode},
        {"--containment-comparison", &EwsHandler::_containment_comparison},
        {"--item-property-path", &EwsHandler::_item_property_path},
        {"--search-type", &EwsHandler::_search_type},
        {"--search-filter", &EwsHandler::_search_filter},
        {"--search-filter-type", &EwsHandler::_search_filter_type},
        {"--action", &EwsHandler::_action}};

    for (size_t i = 0; i < args.size(); i++)
    {
        std::string val = args[i + 1];
        if (val == "--help" || val == "-h")
        {
            _show_help = true;
            break;
        }
        else if (val == "--version" || val == "-v")
        {
            _show_version = true;
            break;
        }
        // Wenn das aktuelle Argument in paramMap ist und es gibt ein nächstes Argument
        else if (paramMap.find(args[i]) != paramMap.end() && i + 1 < args.size())
        {
            *paramMap[args[i]] = val; // Weise der zugehörigen Variable den Wert des nächsten Arguments zu
            i++;                      // Überspringe den Wert
        }
    }
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
        auto data = EwsMailData(service, msg);
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
        auto data = EwsMailData(service, msg);
        list.push_back(data);
        auto j = data.to_json();
        std::cout << j.dump(4) << std::endl;
    }
}

void EwsHandler::search_mails(ews::service &service)
{
    auto standard_folder_id = EwsHandler::get_standard_folder_id();
    std::vector<ews::item_id> item_ids;

    auto search_expression = EwsHandler::get_search_expression();
    item_ids = service.find_item(standard_folder_id, search_expression);

    std::vector<EwsMailData> list = std::vector<EwsMailData>();
    nlohmann::json j;

    for (const auto &id : item_ids)
    {
        auto msg = service.get_message(id);
        auto data = EwsMailData(service, msg);
        j += data.to_json();
    }
    std::cout << j.dump(4) << std::endl;
}

void EwsHandler::search_mails(ews::service &service, std::string &folder_path)
{
    auto folder = EwsHandler::find_folder(service, folder_path);
    std::vector<ews::item_id> item_ids;

    auto search_expression = EwsHandler::get_search_expression();
    item_ids = service.find_item(folder.get_folder_id(), search_expression);

    std::vector<EwsMailData> list = std::vector<EwsMailData>();

    for (const auto &id : item_ids)
    {
        auto msg = service.get_message(id);
        auto data = EwsMailData(service, msg, folder);
        list.push_back(data);
        auto j = data.to_json();
        std::cout << j.dump(4) << std::endl;
    }
}

void EwsHandler::move_mail(ews::service &service, std::string &item_id, std::string &change_key, std::string &folder_path, std::string &dest_folder_path)
{
    auto folder = EwsHandler::find_folder(service, folder_path);
    auto dest_folder = EwsHandler::find_folder(service, dest_folder_path);
    ews::item_id id(item_id, change_key);

    auto moved_id = service.move_item(id, dest_folder.get_folder_id());
    auto msg = service.get_message(moved_id);
    auto data = EwsMailData(service, msg, dest_folder);
    std::cout << data.to_json().dump(4) << std::endl;
}

void EwsHandler::move_mails(ews::service &service, std::string &folder_path, std::string &dest_folder_path)
{
    auto folder = EwsHandler::find_folder(service, folder_path);
    auto dest_folder = EwsHandler::find_folder(service, dest_folder_path);

    auto search_expression = EwsHandler::get_search_expression();
    auto item_ids = service.find_item(folder.get_folder_id(), search_expression);

    service.move_item(item_ids, dest_folder.get_folder_id());

    std::vector<EwsMailData> list = std::vector<EwsMailData>();

    for (const auto &id : item_ids)
    {
        auto msg = service.get_message(id);
        auto data = EwsMailData(service, msg, dest_folder);
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
    if (_show_help)
    {
        show_help();
        return;
    }
    if (_show_version)
    {
        show_version();
        return;
    }

    std::string a = EwsHandler::_action;

    if (a == "create_folder")
    {
        auto folder = EwsHandler::create_folder(service, EwsHandler::_folder_path);
        std::cout << EwsFolderData(folder, EwsHandler::_folder_path).to_json().dump() << std::endl;
    }
    else if (a == "find_folder")
    {
        // TODO find folder by name only
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
    else if (a == "search_mails")
    {
        if (EwsHandler::_folder_path == "")
        {
            EwsHandler::search_mails(service);
        }
        else
        {
            EwsHandler::search_mails(service, EwsHandler::_folder_path);
        }
    }
    else if (a == "move_mail")
    {
        EwsHandler::move_mail(service, EwsHandler::_item_id, EwsHandler::_change_key, EwsHandler::_folder_path, EwsHandler::_dest_folder_path);
    }
    else if (a == "move_mails")
    {
        EwsHandler::move_mails(service, EwsHandler::_folder_path, EwsHandler::_dest_folder_path);
    }
    else
    {
        throw std::runtime_error("No valid action");
    }
}

void EwsHandler::show_help()
{
    std::cout << "Usage: ews-cli [options] [arguments]"
              << std::endl
              << std::endl
              << "Description:"
              << std::endl
              << "  This program is a CLI interface for Mircosoft EWS (Exchange Web Services)."
              << std::endl
              << "  The results are currently written exclusively in stdout.."
              << std::endl
              << std::endl
              << "Options:"
              << std::endl
              << "  -h, --help          Show this help message and exit."
              << std::endl
              << "  -v, --version       Show program version and exit."
              << std::endl
              << "  --url               EWS url of the server (e.g. https://owa.example.com/EWS/Exchange.asmx). (always required)"
              << std::endl
              << "  --domain            EWS / Windows Domain. (always required)"
              << std::endl
              << "  --user              User name with which you want to log in. (always required)"
              << std::endl
              << "  --password          Password of user. (always required)"
              << std::endl
              << "  --mailbox           Name of the e-mail account within the EWS to be accessed (user must have access to it). (optional)"
              << std::endl
              << "  --action            Decides which EWS action is executed. (See Actions below)"
              << std::endl
              << "  --standard-folder   Sets the standard start folder (default inbox)."
              << std::endl
              << "                      Supported: inbox, junk, drafts, sent"
              << std::endl
              << "  --folder-path       Defines the order path for various actions. (/ seperated)"
              << std::endl
              << "                      If option is not used than the standard folder will be used"
              << std::endl
              << "  --dest-folder-path  Defines the order path for various actions (copy, move operations). (/ seperated)"
              << std::endl
              << std::endl
              << "Actions:"
              << std::endl
              << "  get_mails        Get all mails of folder by folder path OR in standard folder"
              << std::endl
              << std::endl
              << "Examples:"
              << std::endl
              << "  ews-cli --version"
              << std::endl;
}

void EwsHandler::show_version()
{
    std::cout << "Version: ews-cli " << _version << std::endl;
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
    else
    {
        return ews::containment_mode::substring;
    }
}

ews::containment_comparison EwsHandler::get_containment_comparison()
{
    auto mode = EwsHandler::_containment_comparison;
    if (mode == "exact")
    {
        return ews::containment_comparison::exact;
    }
    else if (mode == "ignore_case")
    {
        return ews::containment_comparison::ignore_case;
    }
    else if (mode == "ignore_non_spacing_characters")
    {
        return ews::containment_comparison::ignore_non_spacing_characters;
    }
    else if (mode == "loose")
    {
        return ews::containment_comparison::loose;
    }
    else
    {
        return ews::containment_comparison::ignore_case;
    }
}

ews::property_path EwsHandler::get_item_property_path()
{
    auto mode = EwsHandler::_item_property_path;

    if (mode == "archive_tag")
    {
        return ews::item_property_path::archive_tag;
    }
    else if (mode == "attachments")
    {
        return ews::item_property_path::attachments;
    }
    else if (mode == "block_status")
    {
        return ews::item_property_path::block_status;
    }
    else if (mode == "body")
    {
        return ews::item_property_path::body;
    }
    else if (mode == "categories")
    {
        return ews::item_property_path::categories;
    }
    else if (mode == "conversation_id")
    {
        return ews::item_property_path::conversation_id;
    }
    else if (mode == "culture")
    {
        return ews::item_property_path::culture;
    }
    else if (mode == "date_time_created")
    {
        return ews::item_property_path::date_time_created;
    }
    else if (mode == "date_time_received")
    {
        return ews::item_property_path::date_time_received;
    }
    else if (mode == "date_time_sent")
    {
        return ews::item_property_path::date_time_sent;
    }
    else if (mode == "display_cc")
    {
        return ews::item_property_path::display_cc;
    }
    else if (mode == "display_to")
    {
        return ews::item_property_path::display_to;
    }
    else if (mode == "effective_rights")
    {
        return ews::item_property_path::effective_rights;
    }
    else if (mode == "entity_extraction_result")
    {
        return ews::item_property_path::entity_extraction_result;
    }
    else if (mode == "flag")
    {
        return ews::item_property_path::flag;
    }
    else if (mode == "grouping_action")
    {
        return ews::item_property_path::grouping_action;
    }
    else if (mode == "has_attachments")
    {
        // is_equal_to, true|false
        return ews::item_property_path::has_attachments;
    }
    else if (mode == "has_blocked_images")
    {
        return ews::item_property_path::has_blocked_images;
    }
    else if (mode == "icon_index")
    {
        return ews::item_property_path::icon_index;
    }
    else if (mode == "importance")
    {
        return ews::item_property_path::importance;
    }
    else if (mode == "in_reply_to")
    {
        return ews::item_property_path::in_reply_to;
    }
    else if (mode == "instance_key")
    {
        return ews::item_property_path::instance_key;
    }
    else if (mode == "internet_message_headers")
    {
        return ews::item_property_path::internet_message_headers;
    }
    else if (mode == "is_associated")
    {
        return ews::item_property_path::is_associated;
    }
    else if (mode == "is_clutter")
    {
        return ews::item_property_path::is_clutter;
    }
    else if (mode == "is_draft")
    {
        return ews::item_property_path::is_draft;
    }
    else if (mode == "is_from_me")
    {
        return ews::item_property_path::is_from_me;
    }
    else if (mode == "is_resend")
    {
        return ews::item_property_path::is_resend;
    }
    else if (mode == "is_submitted")
    {
        return ews::item_property_path::is_submitted;
    }
    else if (mode == "is_unmodified")
    {
        return ews::item_property_path::is_unmodified;
    }
    else if (mode == "item_class")
    {
        return ews::item_property_path::item_class;
    }
    else if (mode == "item_id")
    {
        return ews::item_property_path::item_id;
    }
    else if (mode == "last_modified_name")
    {
        return ews::item_property_path::last_modified_name;
    }
    else if (mode == "last_modified_time")
    {
        return ews::item_property_path::last_modified_time;
    }
    else if (mode == "mime_content")
    {
        return ews::item_property_path::mime_content;
    }
    else if (mode == "mime_content_utf8")
    {
        return ews::item_property_path::mime_content_utf8;
    }
    else if (mode == "next_predicted_action")
    {
        return ews::item_property_path::next_predicted_action;
    }
    else if (mode == "normalized_body")
    {
        return ews::item_property_path::normalized_body;
    }
    else if (mode == "parent_folder_id")
    {
        return ews::item_property_path::parent_folder_id;
    }
    else if (mode == "policy_tag")
    {
        return ews::item_property_path::policy_tag;
    }
    else if (mode == "predicted_action_reasons")
    {
        return ews::item_property_path::predicted_action_reasons;
    }
    else if (mode == "preview")
    {
        return ews::item_property_path::preview;
    }
    else if (mode == "reminder_due_by")
    {
        return ews::item_property_path::reminder_due_by;
    }
    else if (mode == "reminder_is_set")
    {
        return ews::item_property_path::reminder_is_set;
    }
    else if (mode == "reminder_minutes_before_start")
    {
        return ews::item_property_path::reminder_minutes_before_start;
    }
    else if (mode == "reminder_next_time")
    {
        return ews::item_property_path::reminder_next_time;
    }
    else if (mode == "response_objects")
    {
        return ews::item_property_path::response_objects;
    }
    else if (mode == "retention_date")
    {
        return ews::item_property_path::retention_date;
    }
    else if (mode == "rights_management_license_data")
    {
        return ews::item_property_path::rights_management_license_data;
    }
    else if (mode == "sensitivity")
    {
        return ews::item_property_path::sensitivity;
    }
    else if (mode == "size")
    {
        return ews::item_property_path::size;
    }
    else if (mode == "store_entry_id")
    {
        return ews::item_property_path::store_entry_id;
    }
    else if (mode == "subject")
    {
        // contains
        return ews::item_property_path::subject;
    }
    else if (mode == "text_body")
    {
        return ews::item_property_path::text_body;
    }
    else if (mode == "unique_body")
    {
        return ews::item_property_path::unique_body;
    }
    else if (mode == "web_client_edit_from_query_string")
    {
        return ews::item_property_path::web_client_edit_from_query_string;
    }
    else if (mode == "web_client_read_from_query_string")
    {
        return ews::item_property_path::web_client_read_from_query_string;
    }
    else
    {
        return ews::item_property_path::subject;
    }
}

ews::search_expression EwsHandler::get_search_expression()
{
    auto item_property_path = EwsHandler::get_item_property_path();
    auto containment_mode = EwsHandler::get_containment_mode();
    auto containment_comparison = EwsHandler::get_containment_comparison();
    auto search_filter = EwsHandler::_search_filter;
    auto search_type = EwsHandler::_search_type;
    auto filter_type = EwsHandler::_search_filter_type;

    if (search_type == "is_equal_to")
    {
        auto data = EwsHandler::type_cast(search_filter, EwsHandler::_search_filter_type);
        return EwsHandler::get_is_equal_to(item_property_path, search_filter, filter_type);
    }
    if (search_type == "is_not_equal_to")
    {
        auto data = EwsHandler::type_cast(search_filter, EwsHandler::_search_filter_type);
        return EwsHandler::get_is_not_equal_to(item_property_path, search_filter, filter_type);
    }
    if (search_type == "is_greater_than")
    {
        auto data = EwsHandler::type_cast(search_filter, EwsHandler::_search_filter_type);
        return EwsHandler::get_is_greater_than(item_property_path, search_filter, filter_type);
    }
    if (search_type == "is_greater_than_or_equal_to")
    {
        auto data = EwsHandler::type_cast(search_filter, EwsHandler::_search_filter_type);
        return EwsHandler::get_is_greater_than_or_equal_to(item_property_path, search_filter, filter_type);
    }
    if (search_type == "is_less_than")
    {
        auto data = EwsHandler::type_cast(search_filter, EwsHandler::_search_filter_type);
        return EwsHandler::get_is_less_than(item_property_path, search_filter, filter_type);
    }
    if (search_type == "is_less_than_or_equal_to")
    {
        auto data = EwsHandler::type_cast(search_filter, EwsHandler::_search_filter_type);
        return EwsHandler::get_is_less_than_or_equal_to(item_property_path, search_filter, filter_type);
    }
    else
    {
        const char *filter = EwsHandler::_search_filter.c_str();
        return ews::contains(item_property_path,
                             filter,
                             containment_mode,
                             containment_comparison);
    }
}

ews::is_equal_to EwsHandler::get_is_equal_to(ews::property_path property_path, const std::string &value, std::string type)
{
    auto data = EwsHandler::type_cast(value, type);
    if (type == "int")
    {
        return ews::is_equal_to(property_path, std::get<int>(data));
    }
    if (type == "bool")
    {
        return ews::is_equal_to(property_path, std::get<bool>(data));
    }
    else if (type == "date_time")
    {
        return ews::is_equal_to(property_path, std::get<ews::date_time>(data));
    }
    else
    {
        return ews::is_equal_to(property_path, std::get<const char *>(data));
    }
}

ews::is_not_equal_to EwsHandler::get_is_not_equal_to(ews::property_path property_path, const std::string &value, std::string type)
{
    auto data = EwsHandler::type_cast(value, type);
    if (type == "int")
    {
        return ews::is_not_equal_to(property_path, std::get<int>(data));
    }
    if (type == "bool")
    {
        return ews::is_not_equal_to(property_path, std::get<bool>(data));
    }
    else if (type == "date_time")
    {
        return ews::is_not_equal_to(property_path, std::get<ews::date_time>(data));
    }
    else
    {
        return ews::is_not_equal_to(property_path, std::get<const char *>(data));
    }
}

ews::is_greater_than EwsHandler::get_is_greater_than(ews::property_path property_path, const std::string &value, std::string type)
{
    auto data = EwsHandler::type_cast(value, type);
    if (type == "int")
    {
        return ews::is_greater_than(property_path, std::get<int>(data));
    }
    if (type == "bool")
    {
        return ews::is_greater_than(property_path, std::get<bool>(data));
    }
    else if (type == "date_time")
    {
        return ews::is_greater_than(property_path, std::get<ews::date_time>(data));
    }
    else
    {
        return ews::is_greater_than(property_path, std::get<const char *>(data));
    }
}

ews::is_greater_than_or_equal_to EwsHandler::get_is_greater_than_or_equal_to(ews::property_path property_path, const std::string &value, std::string type)
{
    auto data = EwsHandler::type_cast(value, type);
    if (type == "int")
    {
        return ews::is_greater_than_or_equal_to(property_path, std::get<int>(data));
    }
    if (type == "bool")
    {
        return ews::is_greater_than_or_equal_to(property_path, std::get<bool>(data));
    }
    else if (type == "date_time")
    {
        return ews::is_greater_than_or_equal_to(property_path, std::get<ews::date_time>(data));
    }
    else
    {
        return ews::is_greater_than_or_equal_to(property_path, std::get<const char *>(data));
    }
}

ews::is_less_than EwsHandler::get_is_less_than(ews::property_path property_path, const std::string &value, std::string type)
{
    auto data = EwsHandler::type_cast(value, type);
    if (type == "int")
    {
        return ews::is_less_than(property_path, std::get<int>(data));
    }
    if (type == "bool")
    {
        return ews::is_less_than(property_path, std::get<bool>(data));
    }
    else if (type == "date_time")
    {
        return ews::is_less_than(property_path, std::get<ews::date_time>(data));
    }
    else
    {
        return ews::is_less_than(property_path, std::get<const char *>(data));
    }
}

ews::is_less_than_or_equal_to EwsHandler::get_is_less_than_or_equal_to(ews::property_path property_path, const std::string &value, std::string type)
{
    auto data = EwsHandler::type_cast(value, type);
    if (type == "int")
    {
        return ews::is_less_than_or_equal_to(property_path, std::get<int>(data));
    }
    if (type == "bool")
    {
        return ews::is_less_than_or_equal_to(property_path, std::get<bool>(data));
    }
    else if (type == "date_time")
    {
        return ews::is_less_than_or_equal_to(property_path, std::get<ews::date_time>(data));
    }
    else
    {
        return ews::is_less_than_or_equal_to(property_path, std::get<const char *>(data));
    }
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

std::variant<int, bool, const char *, ews::date_time> EwsHandler::type_cast(const std::string &value, std::string type)
{
    if (type == "int")
    {
        try
        {
            int intValue = std::stoi(value);
            return intValue;
        }
        catch (const std::invalid_argument &e)
        {
            throw std::invalid_argument("Invalid integer value: " + value);
        }
        catch (const std::out_of_range &e)
        {
            throw std::out_of_range("Integer value out of range: " + value);
        }
    }
    else if (type == "bool")
    {
        if (value == "true" || value == "1")
        {
            return true;
        }
        else if (value == "false" || value == "0")
        {
            return false;
        }
        else
        {
            throw std::invalid_argument("Invalid boolean value: " + value);
        }
    }
    else if (type == "const char*" || type == "text" || type == "string")
    {
        const char *val = value.c_str();
        return val;
    }
    else if (type == "date_time" || type == "date")
    {
        try
        {
            int intValue = std::stoi(value);
            ews::date_time d;
            d.from_epoch(intValue);
            return d;
        }
        catch (const std::invalid_argument &e)
        {
            throw std::invalid_argument("Invalid date_time epoch value: " + value);
        }
        catch (const std::out_of_range &e)
        {
            throw std::out_of_range("Date_time epoch int value out of range: " + value);
        }
    }
    else
    {
        const char *val = value.c_str();
        return val;
    }
}