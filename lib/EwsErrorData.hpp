/**
 * Data class for output
 */
#include <string>
#include <vector>
#include <ews/ews.hpp>
#include <nlohmann/json.hpp>

class EwsErrorData
{
public:
    EwsErrorData(std::string error_msg);
    std::string get_error_msg();

    nlohmann::json to_json();

private:
    std::string _error_msg;
};

EwsErrorData::EwsErrorData(std::string error_msg) {
    _error_msg = error_msg;
}

std::string EwsErrorData::get_error_msg() {
    return _error_msg;
}

nlohmann::json EwsErrorData::to_json()
{
    nlohmann::json j;
    j["error"] = _error_msg;
    return j;
}

