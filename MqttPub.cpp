#include <chrono>
#include "MosquittoCpp.h"
#include "Logger.h"
#include <iostream>
#include <cstring>
#include <thread>

namespace
{
    std::size_t timeStampSize = 8;
    std::string sessionId = "MqttPub";
    std::string host = "localhost";
    std::string testTopic = "varroc/test";
    std::string endTestTopic = "varroc/endTest";
}

class MqttPub : IMosquittoListener
{
public:
   MqttPub(uint16_t in, uint16_t cnt, uint16_t size) :
      interval(in), count(cnt), buffer(size < timeStampSize ? timeStampSize : size, 0xFF)
   {}
   
   void Init()
   {
      Logger::Initialize(sessionId);
      mosq.Init(sessionId);
      mosq.SetListener(this);
      mosq.Connect(host);
   }

   void OnConnect(int res) override
   {
      std::cout << "Connection callback, result" << res << std::endl;
      Logger::Log("Connection result:", res);
   }

   void OnMessage(const mosquitto_message*) override
   {}

   void StartTest()
   {
      std::size_t idx{};
      while(++idx <= count)
      {
         SetTimeStamp();
         auto res = mosq.Publish(testTopic, buffer);
         Logger::Log("Publish result:", res);
         std::this_thread::sleep_for(std::chrono::milliseconds(interval));  
      }
      mosq.Publish(endTestTopic);
   }

protected:
void SetTimeStamp()
{
   using namespace std::chrono;
   auto timeStamp = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();

   for(std::size_t idx = 0 ; idx < sizeof(timeStamp); ++idx)
   {
      uint8_t* start = (uint8_t*)&timeStamp;
      buffer[idx] = *(start + idx);
   }
}

   uint16_t interval;
   uint16_t count;
   MosquittoCpp mosq;
   std::vector<uint8_t> buffer;
};

int main()
{
   MqttPub mosq(10, 100, 500);
   mosq.Init();
   mosq.StartTest();
   return 0;
}