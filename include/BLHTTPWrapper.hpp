#ifndef BLHTTPWRAPPER_HPP
#define BLHTTPWRAPPER_HPP

#include "json.hpp"

#include "httpserver.h"
#include <iostream>
#include <cstring>
#include <unordered_map>
#include <bitset>

using json = nlohmann::json;

namespace BuildLogicHTTP {
  struct ParsedRequest {
    int value = 0;
    bool success = false;
    int statusCode = 400;
    std::string error;
    std::unordered_map<std::string, std::string> headers;
  };

  int Init(int PORT, void (*handler)(struct http_request_s*));
  ParsedRequest ParsePost(struct http_request_s* request);
  void SendResponse(struct http_request_s* request, int value); 
}

#endif
