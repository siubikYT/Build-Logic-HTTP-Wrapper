#define HTTPSERVER_IMPL
#include "BLHTTPWrapper.hpp"

int BuildLogicHTTP::Init(int PORT, void (*handler)(struct http_request_s*)) {
  struct http_server_s* server = http_server_init(PORT, handler);
  http_server_listen(server);
  return 0;
}
