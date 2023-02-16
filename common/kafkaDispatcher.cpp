#include "kafkaDispatcher.h"

std::string configFile =  "./config.ini";
int delivery_counter = 0;

kafkaDispatcher::kafkaDispatcher(std::string confPath, rd_kafka_t *producer, std::string topic)
{
  char errstr[512];
  kafkaDispatcher::topic = topic;
  
  if (!(conf = read_config(confPath.c_str())))
        throw std::runtime_error("Failed to read or processing conf file");

  /* Set up a delivery report callback that will be triggered
   * from poll() or flush() for the final delivery status of
   * each message produced. */
  rd_kafka_conf_set_dr_msg_cb(conf, delivery_callback);

  /* Create producer.
   * A successful call assumes ownership of \p conf. */
  producer = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
  if (!producer) {
    fprintf(stderr, "Failed to create producer: %s\n", errstr);
    rd_kafka_conf_destroy(conf);
    throw std::runtime_error(errstr);
  }

  /* Create the topic. */
  if (create_topic(producer, topic.c_str(), 1) == -1) {
    rd_kafka_destroy(producer);
    throw std::runtime_error("Error to create topic");
  }
}

void kafkaDispatcher::delivery_callback (rd_kafka_t *rk, const rd_kafka_message_t *rkmessage, void *opaque) {
  int *delivery_counterp = (int *)rkmessage->_private; /* V_OPAQUE */

  if (rkmessage->err) {
    fprintf(stderr, "Delivery failed for message %.*s: %s\n",
            (int)rkmessage->len, (const char *)rkmessage->payload,
            rd_kafka_err2str(rkmessage->err));
  } else {
    fprintf(stderr,
            "Sucesso enviando [%s] :::partition[%d] at offset [%"PRId64
            "] in %.2fms: %.*s\n",
            rd_kafka_topic_name(rkmessage->rkt),
            (int)rkmessage->partition,
            rkmessage->offset,
            (float)rd_kafka_message_latency(rkmessage) / 1000.0,
            (int)rkmessage->len, (const char *)rkmessage->payload);    
    (*delivery_counterp)++;
  }
}

kafkaDispatcher::~kafkaDispatcher() {
  delete producer;
  delete conf;
}

void kafkaDispatcher::send(std::string key, std::string value) {
  rd_kafka_resp_err_t err;

  /* Asynchronous produce */
  err = rd_kafka_producev(
          producer,
          RD_KAFKA_V_TOPIC(topic.c_str()),
          RD_KAFKA_V_KEY(key.c_str(), key.length()),
          RD_KAFKA_V_VALUE((char *)value.c_str(), value.length()),
          RD_KAFKA_V_OPAQUE(&delivery_counter),
          RD_KAFKA_V_END);

  if (err) {
    fprintf(stderr, "\tProduce failed: %s\n", rd_kafka_err2str(err));
  }

  /* Poll for delivery report callbacks to know the final
    * delivery status of previously produced messages. */
  rd_kafka_poll(producer, 5);

  delivery_counter++;
}
