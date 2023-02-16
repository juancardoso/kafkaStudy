#include <stdio.h>
#include <signal.h>
#include <string>
#include <stdexcept>

#include <librdkafka/rdkafka.h>

#include "common.h"
#include "json.h"


namespace kafkaDispatcher {
  class kafkaDispatcher
  {
  private:
    std::string configFile;
    rd_kafka_t *producer;
    rd_kafka_conf_t *conf;
    std::string topic;
    int delivery_counter;
    

  public:
    kafkaDispatcher(std::string confPath, rd_kafka_t *producer, std::string topic);
    ~kafkaDispatcher();
    static void delivery_callback (rd_kafka_t *rk, const rd_kafka_message_t *rkmessage, void *opaque);
    void send(std::string key, std::string value);
  };
} //namespace
