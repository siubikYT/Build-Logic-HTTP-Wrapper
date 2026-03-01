#ifndef BLHTTPWRAPPER_HPP
#define BLHTTPWRAPPER_HPP

#include "include/json.hpp"
#include "include/httplib.h"
#include <iostream>
#include <cstring>

namespace BuildLogicHTTP {
  int Init() {
    httplib::Server server;

    server.Post("", [&](const Request& req, Response& res) {

    });

    return 0;
  } 
}

#endif
