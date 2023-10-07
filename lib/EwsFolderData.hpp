/**
 * Data class for output
 */
#include <string>
#include <vector>
#include <ews/ews.hpp>
#include <nlohmann/json.hpp>

class EwsFolderData
{
public:
    EwsFolderData();
    EwsFolderData(ews::folder &folder);
    EwsFolderData(const ews::folder &folder);
    EwsFolderData(ews::folder &folder, std::string &path);

    nlohmann::json to_json();

    int get_items_count();
    std::string get_display_name();
    std::string get_path();

private:
    int _items_count;
    std::string _display_name;
    std::string _path;
};

EwsFolderData::EwsFolderData()
{
}

EwsFolderData::EwsFolderData(const ews::folder &folder)
{
    _items_count = folder.get_total_count();
    _display_name = folder.get_display_name();
}

EwsFolderData::EwsFolderData(ews::folder &folder)
{
    _items_count = folder.get_total_count();
    _display_name = folder.get_display_name();
}

EwsFolderData::EwsFolderData(ews::folder &folder, std::string &path)
{
    _items_count = folder.get_total_count();
    _display_name = folder.get_display_name();
    _path = path;
}

int EwsFolderData::get_items_count()
{
    return _items_count;
}

std::string EwsFolderData::get_display_name()
{
    return _display_name;
}

std::string EwsFolderData::get_path()
{
    return _path;
}

nlohmann::json EwsFolderData::to_json()
{
    nlohmann::json j;
    j["display_name"] = _display_name;
    j["items_count"] = _items_count;
    j["path"] = _path;
    return j;
}