/**
 * Data class for output
 */
#include <string>
#include <vector>
#include <ews/ews.hpp>
#include <nlohmann/json.hpp>

class EwsAttachmentData
{
public:
    EwsAttachmentData();
    EwsAttachmentData(ews::attachment &attachment);
    EwsAttachmentData(const ews::attachment &attachment);

    nlohmann::json to_json();

private:
    std::string _display_name;
    std::string _path;

    std::string _id;
    std::string _content_id;
    std::string _name;
    std::string _mime_content;
    std::string _content_type;
    /**
     * item or file
    */
    std::string _type;
    /**
     * base64 string
     */
    std::string _content;
    size_t _content_size;
};

EwsAttachmentData::EwsAttachmentData()
{
}

EwsAttachmentData::EwsAttachmentData(const ews::attachment &attachment)
{
    _id = attachment.id().id();
    _content_id = attachment.content_id();
    _content = attachment.content();
    _content_size = attachment.content_size();
    _content_type = attachment.content_type();
    _name = attachment.name();
    _mime_content = attachment.get_mime_content();
    auto type = attachment.get_type();
    if (type == ews::attachment::type::file) {
        _type = "file";
    } else {
        _type = "item";
    }

}

EwsAttachmentData::EwsAttachmentData(ews::attachment &attachment)
{
    _id = attachment.id().id();
    _content_id = attachment.content_id();
    _content = attachment.content();
    _content_size = attachment.content_size();
    _content_type = attachment.content_type();
    _name = attachment.name();
    _mime_content = attachment.get_mime_content();
    auto type = attachment.get_type();
    if (type == ews::attachment::type::file) {
        _type = "file";
    } else {
        _type = "item";
    }
    attachment.write_content_to_file("./attachment/test.pdf");
}

nlohmann::json EwsAttachmentData::to_json()
{
    nlohmann::json j;
    j["id"] = _id;
    j["content_id"] = _content_id;
    j["content"] = _content;
    j["content_size"] = _content_size;
    j["content_type"] = _content_type;
    j["name"] = _name;
    j["mime_content"] = _mime_content;
    j["type"] = _type;
    return j;
}