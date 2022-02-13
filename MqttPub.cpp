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
   uint8_t expectedParameters = 6;

   struct options
   {
      uint16_t interval;
      uint32_t count;
      uint32_t size;
   };

   void PrintHelp()
   {
      std::cout << "-------------------------------------------------------------" << std::endl;
      std::cout << "Required options:                                            " << std::endl;
      std::cout << "-i     interval [ms]                                         " << std::endl;
      std::cout << "-c     message number                                        " << std::endl;
      std::cout << "-s     message size in bytes                                 " << std::endl;
      std::cout << "-------------------------------------------------------------" << std::endl;
   }

   bool ParseOptions(int argc, char** argv, options& opt)
   {
      if(argc != expectedParameters + 1)
      {
         return false;
      }

      for(std::size_t idx = 1; idx < argc; idx= idx + 2)
      {
         if(strcmp(argv[idx], "-i") == 0)
         {
            opt.interval = std::atoi(argv[idx + 1]);
         }
         else if(strcmp(argv[idx], "-c") == 0)
         {
            opt.count = std::atoi(argv[idx + 1]);
         }
         else if(strcmp(argv[idx], "-s") == 0)
         {
            opt.size = std::atoi(argv[idx + 1]);
         }
         else
         {
            return false;
         }
      }
      return true;
   }
}

class MqttPub : IMosquittoListener
{
public:
   MqttPub(uint16_t in, uint32_t cnt, uint32_t size) :
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

         Logger::Log("Publish topic", testTopic, "result:", res);
         std::this_thread::sleep_for(std::chrono::milliseconds(interval));  
      }
      auto res = mosq.Publish(endTestTopic, 1);
      Logger::Log("Test finished. Publish result:", res);

      std::this_thread::sleep_for(std::chrono::milliseconds(2000));
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
   uint32_t count;
   MosquittoCpp mosq;
   std::vector<uint8_t> buffer;
};

int main(int argc, char** argv)
{
   options opt;
   bool result = ParseOptions(argc, argv, opt);

   if(!result)
   {
      PrintHelp();
      return 0;
   }

   std::cout << "Test app running with options : " << std::endl << "interval:" << opt.interval 
                                                   << std::endl << "message size:" << opt.size
                                                   << std::endl << "number of mesages:" << opt.count << std::endl;
            
   MqttPub mosq(opt.interval, opt.count, opt.size);
   mosq.Init();
   mosq.StartTest();
   return 0;
}