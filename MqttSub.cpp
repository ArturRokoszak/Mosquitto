#include <chrono>
#include "MosquittoCpp.h"
#include "Logger.h"
#include <iostream>
#include <cstring>
#include <thread>
#include <algorithm>
#include <numeric>
#include <iterator>

namespace
{
    std::string sessionId = "MqttSub";
    std::string host = "localhost";
    std::string testTopic = "varroc/test";
    std::string endTestTopic = "varroc/endTest";
}

class MqttSub : IMosquittoListener
{
public:
   MqttSub()
   {}
   
   void Init()
   {
      Logger::Initialize(sessionId);
      mosq.Init(sessionId);
      mosq.SetListener(this);
      mosq.Connect(host);
      mosq.Subscribe(testTopic);
      mosq.Subscribe(endTestTopic);
   }

   void StartTest()
   {
      mosq.LoopStartForever();
   }

   void OnConnect(int res) override
   {
      std::cout << "Connection callback, result" << res << std::endl;
      Logger::Log("Connection result:", res);
   }

   void OnMessage(const mosquitto_message* msg) override
   {
      using namespace std::chrono;
      auto currentTime = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
      std::string topic = msg->topic;
      if(topic == testTopic)
      {
         uint64_t timeStamp = *((uint64_t*)msg->payload);
         auto timeDiff = currentTime - timeStamp;
         std::cout << "Received msg. Latency : " << timeDiff << std::endl;
         values.push_back(timeDiff);
      }
      else if(topic == endTestTopic)
      {
         auto min = std::min_element(values.begin(), values.end());
         auto max = std::max_element(values.begin(), values.end());
         auto avg = std::accumulate(values.begin(), values.end(), 0)/values.size();
         std::cout << "Min:" << *min << " Max:" << *max << " Avg:" << avg << std::endl;
         mosq.Disconnect();
      }
   }

protected:
   MosquittoCpp mosq;
   std::vector<uint32_t> values;
};

int main()
{
   MqttSub mosq;
   mosq.Init();
   mosq.StartTest();

   return 0;
}