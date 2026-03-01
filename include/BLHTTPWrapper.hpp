#ifndef BLHTTPWRAPPER_HPP
#define BLHTTPWRAPPER_HPP

#include "json.hpp"
#include "httplib.h"
#include <iostream>
#include <unordered_map>
#include <bitset>
#include <functional>

using json = nlohmann::json;

namespace BuildLogicHTTP {

  struct ParsedRequest {
    int value = 0;
    bool success = false;
    int statusCode = 400;
    std::string error;
    std::unordered_map<std::string, std::string> headers;
  };

  using RequestHandler = std::function<void(const ParsedRequest&, std::string& responseBody, std::string& contentType, int& statusCode)>;

  int Init(int PORT, RequestHandler handler);
  ParsedRequest ParsePost(const httplib::Request& req); 
  void SendResponse(const ParsedRequest& req, std::string& body, std::string& type, int& statusCode);
}

#endif
