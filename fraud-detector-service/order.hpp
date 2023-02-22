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
    Order(const json& j) {
      from_json(j);
    };

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
      j.at("userId").get_to(this->userId);
      j.at("orderId").get_to(this->orderId);
      j.at("email").get_to(this->email);
      j.at("amount").get_to(this->amount);
    }

    std::string toString() {
      std::string ret = "[UserId:{" + userId + "} - orderId:{" + orderId + "} -"+ 
                                "email:{" + email + "} - amount:{" + std::to_string(amount) + "}]"; 

      return ret;
    }
};