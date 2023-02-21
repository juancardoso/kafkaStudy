#include <stdio.h>
#include <string.h>
#include <cstdlib>

#include "kafkaDispatcher.hpp"
#include "order.hpp"

int main(int argc, char **argv) {
    std::string confPath;
    float amount = 100.00;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <config-file>\n", argv[0]);
        exit(1);
    }

    confPath = argv[1];

    kafkaDispatcher<Order> orderDispatcher(confPath, "ECOMMERCE_NEW_ORDER"); 
    //kafkaDispatcher emailDispatcher(confPath, "ECOMMERCE_SEND_EMAIL"); 

    std::string fixedEmail = "juan@email.com.br";
    for (int i = 0; i < 10; i++) {
        std::string userId = std::to_string(rand());
        std::string orderId = std::to_string(rand());
        amount += 3.5;

        Order order(userId, orderId, amount, fixedEmail);
        orderDispatcher.send(userId, order);
    }

    printf("\nFIM\n");
}