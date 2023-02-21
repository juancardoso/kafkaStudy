#include <string>
#include "json.hpp"

using json = nlohmann::json;

class Order {

private: 
    std::string userId;
    std::string orderId;
    std::string email;
    float amount;

public:
    Order(std::string userId, std::string orderId, float amount, std::string email) {
        this->userId = userId;
        this->orderId = orderId;
        this->amount = amount;
        this->email = email;
    }

    void to_json(json& j) const {
        j = json{{"userId", userId}, {"orderId", orderId}, {"email", email},{"amount", amount}};
    }

    void from_json(const json& j) {
        j.at("userId").get_to(userId);
        j.at("orderId").get_to(orderId);
        j.at("email").get_to(email);
        j.at("amount").get_to(amount);
    }
};