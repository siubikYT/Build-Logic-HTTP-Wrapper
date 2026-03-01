#ifndef BLHTTPWRAPPER_HPP
#define BLHTTPWRAPPER_HPP

#include "json.hpp"

#include "httpserver.h"
#include <iostream>
#include <cstring>

using json = nlohmann::json;

namespace BuildLogicHTTP {
  int Init(int PORT, void (*handler)(struct http_request_s*));
}

#endif
