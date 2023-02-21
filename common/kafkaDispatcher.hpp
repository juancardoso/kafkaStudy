#include <stdio.h>
#include <signal.h>
#include <string>
#include <stdexcept>

#include <librdkafka/rdkafka.h>

#include "common.h"
#include "json.hpp"

using json = nlohmann::json;

template <typename T>

class kafkaDispatcher
{
  private:
    rd_kafka_t *producer;
    rd_kafka_conf_t *conf;
    std::string topic;
    int delivery_counter;
    
  public:
    kafkaDispatcher(std::string confPath, std::string topic) {
      this->delivery_counter = 0;
      this->topic = topic;

      char errstr[512];
      
      if (!(conf = read_config(confPath.c_str())))
            throw std::runtime_error("Failed to read or processing conf file");

      /* Set up a delivery report callback that will be triggered
      * from poll() or flush() for the final delivery status of
      * each message produced. */
      rd_kafka_conf_set_dr_msg_cb(conf, delivery_callback);

      /* Create producer.
      * A successful call assumes ownership of \p conf. */
      this->producer = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
      if (!this->producer) {
        fprintf(stderr, "Failed to create producer: %s\n", errstr);
        rd_kafka_conf_destroy(conf);
        throw std::runtime_error(errstr);
      }

      /* Create the topic. */
      if (create_topic(this->producer, topic.c_str(), 1) == -1) {
        rd_kafka_destroy(this->producer);
        throw std::runtime_error("Error to create topic");
      }
    }

    ~kafkaDispatcher() {
      if(producer != nullptr) rd_kafka_destroy(producer);
      // For some reason, free conf memory is returning: 
      // "munmap_chunk(): invalid pointer"
      //if(conf != nullptr) rd_kafka_conf_destroy(conf);
    }

  static void delivery_callback (rd_kafka_t *rk, const rd_kafka_message_t *rkmessage, void *opaque) {
    int *delivery_counterp = (int *)rkmessage->_private; /* V_OPAQUE */

    if (rkmessage->err) {
      fprintf(stderr, "Delivery failed for message %.*s: %s\n",
              (int)rkmessage->len, (const char *)rkmessage->payload,
              rd_kafka_err2str(rkmessage->err));
    } else {
      fprintf(stderr,
              "Sucesso enviando [%s] :::partition[%d] at offset [%" PRId64
              "] in %.2fms: %.*s\n",
              rd_kafka_topic_name(rkmessage->rkt),
              (int)rkmessage->partition,
              rkmessage->offset,
              (float)rd_kafka_message_latency(rkmessage) / 1000.0,
              (int)rkmessage->len, (const char *)rkmessage->payload);  
      (*delivery_counterp)++;
    }
  }
    
    void send(std::string key, T value) {
      rd_kafka_resp_err_t err;
      std::string message;

      // Serializa o objeto em formato JSON
      json j;
      value.to_json(j);
      
      std::string sJsonValue = j.dump();

      /* Asynchronous produce */
      err = rd_kafka_producev(
              this->producer,
              RD_KAFKA_V_TOPIC(this->topic.c_str()),
              RD_KAFKA_V_KEY(key.c_str(), key.length()),
              RD_KAFKA_V_VALUE((char *)sJsonValue.c_str(), sJsonValue.length()),
              RD_KAFKA_V_OPAQUE(&this->delivery_counter),
              RD_KAFKA_V_END);

      if (err) {
        fprintf(stderr, "\tProduce failed: %s\n", rd_kafka_err2str(err));
      }

      /* Poll for delivery report callbacks to know the final
        * delivery status of previously produced messages. */
      rd_kafka_poll(this->producer, 1000);

      this->delivery_counter++;
    }
};