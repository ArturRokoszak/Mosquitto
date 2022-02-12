cmake_minimum_required(VERSION 3.10)
project(MosquittoDemo)

set(MQTT_CPP MqttCpp)
add_library(${MQTT_CPP} MosquittoCpp.cpp)
target_link_libraries(${MQTT_CPP} mosquitto)

add_executable(MqttPub MqttPub.cpp)
target_link_libraries(MqttPub ${MQTT_CPP})

add_executable(MqttSub MqttSub.cpp)
target_link_libraries(MqttSub ${MQTT_CPP})