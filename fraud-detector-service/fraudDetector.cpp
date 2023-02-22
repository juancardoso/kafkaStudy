#include <string>
#include <librdkafka/rdkafka.h>

#include "order.hpp"
#include "kafkaService.hpp"

int main(int argc, char **argv) {
    rd_kafka_conf_t *conf;
    rd_kafka_t *consumer;
    std::string confPath;
    char errstr[512];


    if (argc != 2) {
        fprintf(stderr, "Usage: %s <config-file>\n", argv[0]);
        exit(1);
    }

    if (!(conf = read_config(argv[1]))) {
        throw std::runtime_error("Failed to read or processing conf file");
    }

    /* Create consumer.
        * A successful call assumes ownership of \p conf. */
    consumer = rd_kafka_new(RD_KAFKA_CONSUMER, conf, errstr, sizeof(errstr));
    if (!consumer) {
        fprintf(stderr, "Failed to create consumer: %s\n", errstr);
        rd_kafka_conf_destroy(conf);
        return -1;
    }

    kafkaService<Order> fraudService(consumer, conf, "ECOMMERCE_NEW_ORDER");

    fraudService.execute();
}