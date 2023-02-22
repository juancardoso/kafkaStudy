#include <string>
#include <librdkafka/rdkafka.h>

#include "common.h"
#include "json.hpp"

using json = nlohmann::json;

template<class T> 
class kafkaService
{
private:
    rd_kafka_t *consumer;
    rd_kafka_conf_t *conf;
    std::string topicName;
    rd_kafka_topic_partition_list_t *topics;

public:
    // Let this way to apply dependency injection by constructor.
    kafkaService(rd_kafka_t *consumer, rd_kafka_conf_t *conf, std::string topicName) {
        this->consumer = consumer;
        this->conf = conf;
        this->topicName = topicName;
        rd_kafka_resp_err_t err;


        /* Redirect all (present and future) partition message queues to the
            * main consumer queue so that they can all be consumed from the
            * same consumer_poll() call. */
        rd_kafka_poll_set_consumer(this->consumer);

        /* Create subscription list.
            * The partition will be ignored by subscribe() */
        topics = rd_kafka_topic_partition_list_new(1);
        rd_kafka_topic_partition_list_add(topics, this->topicName.c_str(),
                                            RD_KAFKA_PARTITION_UA);

        /* Subscribe to topic(s) */
        fprintf(stderr,
                "Subscribed to %s, waiting for assignment and messages...\n"
                "Press Ctrl-C to exit.\n", this->topicName.c_str());
        err = rd_kafka_subscribe(this->consumer, topics);
        rd_kafka_topic_partition_list_destroy(topics);

        if (err) {
                fprintf(stderr, "Subscribe(%s) failed: %s\n",
                        this->topicName.c_str(), rd_kafka_err2str(err));
                rd_kafka_destroy(consumer);
                throw std::runtime_error("Failed to subscribe");
        }
    }

    ~kafkaService() {
        if(consumer != nullptr) {
            rd_kafka_consumer_close(consumer);
            rd_kafka_destroy(consumer);
        } 
    }

    void execute(){
      rd_kafka_message_t *rkm;
      json jsonPayload;

      /* Consume messages */
      while (true) {
        rkm = nullptr;

        /* Poll for a single message or an error event.
            * Use a finite timeout so that Ctrl-C (run==0) is honoured. */
        rkm = rd_kafka_consumer_poll(consumer, 1000);
        if (!rkm) continue;

        if (rkm->err) {
          /* Consumer error: typically just informational. */
          fprintf(stderr, "Consumer error: %s\n",
                  rd_kafka_message_errstr(rkm));
        } else {
          /* Proper message */
          fprintf(stderr,
                  "Received message on %s [%d] "
                  "at offset %" PRId64 ": %.*s\n",
                  rd_kafka_topic_name(rkm->rkt),
                  (int)rkm->partition, rkm->offset,
                  (int)rkm->len, (const char *)rkm->payload);
        
          jsonPayload = (const char*)rkm->payload;
          T t(jsonPayload);
          
          printf("Meu objeto convertido em Json\n\t%s\n", t.toString().c_str());
        }

        rd_kafka_message_destroy(rkm);
      }
    }
};