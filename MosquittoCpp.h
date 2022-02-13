#pragma once

#include <mosquitto.h>
#include <vector>
#include <string>

class IMosquittoListener
{
public:
   virtual void OnConnect(int) = 0;
   virtual void OnMessage(const mosquitto_message*) = 0;
};

class MosquittoCpp
{
public:
   int Init(const std::string& sessionId);
   virtual ~MosquittoCpp();
   void OnConnect(int res);
   void OnMessage(const mosquitto_message*);
   void SetListener(IMosquittoListener*);

   int Connect(const std::string&, int port = 1883);
   int Disconnect();
	int Publish(const std::string&, const std::vector<uint8_t>&, int qos = 0);
   int Publish(const std::string&, int qos = 0);
	int Subscribe(const std::string&, int qos = 0);
   int LoopStart();
   int LoopStartForever();

protected:
   mosquitto* mosq{nullptr};
   IMosquittoListener* listener{nullptr};
};