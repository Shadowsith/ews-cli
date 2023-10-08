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
    EwsMailData(ews::service &service, ews::message &message);

    nlohmann::json to_json();

    std::string get_sender();
    std::string get_subject();
    std::string get_body();

    void set_sender(std::string sender);
    void set_subject(std::string subject);
    void set_body(std::string body);
    void set_importance(ews::importance importance);
    void set_any_recipients(std::vector<ews::mailbox> recipients, std::vector<std::string> &target);

    void from_message(ews::message &message);
    void from_message(ews::service &service, ews::message &message);

private:
    std::string _item_id;
    std::string _sender;
    std::string _subject;
    std::string _body;
    std::string _importance;
    bool _has_attachments;
    std::string _date_time_created;
    std::string _date_time_received;
    std::string _date_time_sent;

    std::vector<std::string> _to_recipients;
    std::vector<std::string> _cc_recipients;
    std::vector<std::string> _bcc_recipients;
    std::vector<EwsAttachmentData> _attachments;
};

EwsMailData::EwsMailData()
{
}

EwsMailData::EwsMailData(ews::service &service, ews::message &message)
{
    from_message(service, message);
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

void EwsMailData::from_message(ews::service &service, ews::message &message)
{
    _item_id = message.get_item_id().id();
    _subject = message.get_subject();
    _body = message.get_body().content();
    _sender = message.get_from().value();
    _date_time_created = message.get_date_time_created().to_string();
    _date_time_received = message.get_date_time_received().to_string();
    _date_time_sent = message.get_date_time_sent().to_string();

    set_importance(message.get_importance());

    set_any_recipients(message.get_to_recipients(), _to_recipients);
    set_any_recipients(message.get_cc_recipients(), _cc_recipients);
    set_any_recipients(message.get_bcc_recipients(), _bcc_recipients);

    _has_attachments = message.has_attachments();
    if (message.has_attachments())
    {
        auto attachments = message.get_attachments();
        for (const auto &a : attachments)
        {
            auto aa = service.get_attachment(a.id());
            _attachments.push_back(EwsAttachmentData(aa));
        }
    }
}

void EwsMailData::set_importance(ews::importance importance)
{
    if (importance == ews::importance::high)
    {
        _importance = "high";
    }
    else if (importance == ews::importance::low)
    {
        _importance = "low";
    }
    else if (importance == ews::importance::normal)
    {
        _importance = "normal";
    }
    else
    {
        _importance = "normal";
    }
}

void EwsMailData::set_any_recipients(std::vector<ews::mailbox> recipients, std::vector<std::string> &target)
{
    for (const auto &r : recipients)
    {
        target.push_back(r.value());
    }
}

nlohmann::json EwsMailData::to_json()
{
    nlohmann::json j;
    j["item_id"] = _item_id;
    j["sender"] = _sender;
    j["subject"] = _subject;
    j["body"] = _body;
    j["to"] = _to_recipients;
    j["cc"] = _cc_recipients;
    j["bcc"] = _bcc_recipients;
    j["date_time_created"] = _date_time_created;
    j["date_time_received"] = _date_time_received;
    j["date_time_sent"] = _date_time_sent;
    j["has_attachements"] = _has_attachments;
    j["priority"] = _importance;

    if (_has_attachments)
    {
        nlohmann::json aj;
        for (auto &a : _attachments)
        {
            aj += a.to_json();
        }
        j["attachments"] = aj;
    }
    else
    {
        j["attachments"] = std::vector<std::string>();
    }
    return j;
}