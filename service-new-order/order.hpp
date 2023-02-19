#include <string>
#include "json.hpp"

using json = nlohmann::json;

class Order {

private: 
    std::string userId;
    std::string orderId;
    float amount;

public:
    Order(std::string userId, std::string orderId, float amount) {
        this->userId = userId;
        this->orderId = orderId;
        this->amount = amount;
    }

    void to_json(json& j) const {
        j = json{{"userId", userId}, {"orderId", orderId}};
    }

    void from_json(const json& j) {
        j.at("userId").get_to(userId);
        j.at("orderId").get_to(orderId);
    }
};