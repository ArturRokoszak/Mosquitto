#include "MosquittoCpp.h"

namespace
{
   void OnConnectWrapper(mosquitto* mosq, void* userData, int result)
   {
      if(userData)
      {
	      auto* msq = reinterpret_cast<MosquittoCpp*>(userData);
	      msq->OnConnect(result);
      }
   }

   void OnMessageWrapper(mosquitto* mosq, void* userData, const mosquitto_message* message)
   {
      if(userData)
      {
	      auto* msq = reinterpret_cast<MosquittoCpp*>(userData);
	      msq->OnMessage(message);
      }
   }
}

int MosquittoCpp::Init(const std::string& sessionId)
{
   mosquitto_lib_init();
   mosq = mosquitto_new(sessionId.c_str(), true, this);
   if(!mosq)
   {
      return MOSQ_ERR_UNKNOWN;
   }
   mosquitto_connect_callback_set(mosq, OnConnectWrapper);
   mosquitto_message_callback_set(mosq, OnMessageWrapper);

   return MOSQ_ERR_SUCCESS;
}

void MosquittoCpp::OnConnect(int res)
{
   if(listener)
   {
      listener->OnConnect(res);
   }
}

void MosquittoCpp::OnMessage(const mosquitto_message* msg)
{
   if(listener)
   {
      listener->OnMessage(msg);
   }
}

void MosquittoCpp::SetListener(IMosquittoListener* lst)
{
   if(lst)
   {
      listener = lst;
   }
}

int MosquittoCpp::Connect(const std::string& host, int port)
{
   return mosquitto_connect(mosq, host.c_str(), port, 60);
}

int MosquittoCpp::Disconnect()
{
   return mosquitto_disconnect(mosq);
}

int MosquittoCpp::Publish(const std::string& topic, const std::vector<uint8_t>& msg)
{
   return mosquitto_publish(mosq, nullptr, topic.c_str(), msg.size(), (const void*)msg.data(), 0, false);
}

int MosquittoCpp::Publish(const std::string& topic)
{
   return mosquitto_publish(mosq, nullptr, topic.c_str(), 0, nullptr, 0, false);
}

int MosquittoCpp::Subscribe(const std::string& topic)
{
   return mosquitto_subscribe(mosq, nullptr, topic.c_str(), 0);
}

int MosquittoCpp::LoopStart()
{
   return mosquitto_loop_start(mosq);
}

int MosquittoCpp::LoopStartForever()
{
   return mosquitto_loop_forever(mosq, -1, 1);
}

MosquittoCpp::~MosquittoCpp()
{
   mosquitto_destroy(mosq);
}