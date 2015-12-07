#include <iostream>
#include "p2psocketsignalingchannel.h"

namespace woogeensample{
  P2PSocketSignalingChannel::P2PSocketSignalingChannel(): io_(new sio::client()){
  }

  void P2PSocketSignalingChannel::AddObserver(woogeen::P2PSignalingChannelInterfaceObserver* observer) {
    observers_.push_back(observer);
  }

  void P2PSocketSignalingChannel::Connect(const std::string& token, std::function<void()> on_success, std::function<void(std::unique_ptr<woogeen::P2PException>)> on_failure){
    std::map<std::string, std::string> query;
    query.insert(std::pair<std::string, std::string>("clientVersion", "3.0"));
    query.insert(std::pair<std::string, std::string>("clientType", "cpp"));
    query.insert(std::pair<std::string, std::string>("token", "30"));  // TODO: parse token to get actual token.
    sio::socket::ptr socket = io_->socket();
    std::string name = "woogeen-message";
    socket->on(name, sio::socket::event_listener_aux([&](std::string const& name, sio::message::ptr const& data, bool has_ack, sio::message::list &ack_resp){
      if (data->get_flag() == sio::message::flag_object){
        std::string msg = data->get_map()["data"]->get_string().data();
        std::string from = data->get_map()["from"]->get_string().data();
        for (auto it = observers_.begin(); it != observers_.end(); ++it){
          (*it)->OnMessage(msg, from);
        };
      }
    }));
    //io_->connect("http://10.239.3.197:8095/", query);  // TODO: parse token to get host.
    io_->connect(token, query);  // TODO: parse token to get host.
  }

  void P2PSocketSignalingChannel::Disconnect(std::function<void()> on_success, std::function<void(std::unique_ptr<woogeen::P2PException>)> on_failure) {

  }

  void P2PSocketSignalingChannel::SendMessage(const std::string& message, const std::string& target_id, std::function<void()> on_success, std::function<void(std::unique_ptr<woogeen::P2PException>)> on_failure) {
    sio::message::ptr jsonObject = sio::object_message::create();
    jsonObject->get_map()["to"] = sio::string_message::create(target_id);
    jsonObject->get_map()["data"] = sio::string_message::create(message);
    io_->socket()->emit("woogeen-message", jsonObject);
  }
}
