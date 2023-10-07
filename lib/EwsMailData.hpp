/**
 * Data class for output
 */
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

class EwsMailData
{
public:
    EwsMailData();
    EwsMailData(ews::message& message);

    nlohmann::json to_json();

    std::string get_sender();
    std::string get_subject();
    std::string get_body();

    void set_sender(std::string sender);
    void set_subject(std::string subject);
    void set_body(std::string body);

    void from_message(ews::message& message);

private:
    std::string _sender;
    std::string _subject;
    std::string _body;
    std::vector<std::string> _recipients;
};

EwsMailData::EwsMailData()
{
}

EwsMailData::EwsMailData(ews::message& message)
{
    from_message(message);
}

std::string EwsMailData::get_sender()
{
    return _sender;
}

std::string EwsMailData::get_subject()
{
    return _subject;
}

std::string EwsMailData::get_body()
{
    return _body;
}

void EwsMailData::set_sender(std::string sender)
{
    _sender = sender;
}

void EwsMailData::set_subject(std::string subject)
{
    _subject = subject;
}

void EwsMailData::set_body(std::string body)
{
    _body = body;
}

void EwsMailData::from_message(ews::message& message)
{
    _subject = message.get_subject();
    _body = message.get_body().content();
    _sender = message.get_from().value();
    _recipients = std::vector<std::string>();
    auto recipients = message.get_to_recipients();
    for(const auto& r : recipients) {
        _recipients.push_back(r.value());
    }
}

nlohmann::json EwsMailData::to_json()
{
    nlohmann::json j;
    j["sender"] = _sender;
    j["subject"] = _subject;
    j["body"] = _body;
    j["recipients"] = _recipients;
    return j;
}