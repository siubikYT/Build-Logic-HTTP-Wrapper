#define HTTPSERVER_IMPL
#include "BLHTTPWrapper.hpp"

int BuildLogicHTTP::Init(int PORT, void (*handler)(struct http_request_s*)) {
  struct http_server_s* server = http_server_init(PORT, handler);
  http_server_listen(server);
  return 0;
}

ParsedRequest BuildLogicHTTP::ParsePost(struct http_request_s* request) {
  ParsedRequest result;

  const char* rawHeader = http_request_header(request, "Content-Type");
  std::string contentType = rawHeader ? std::string(rawHeader) : "";
  if (!contentType.empty()) {
      result.headers["Content-Type"] = contentType;
  } else {
      result.headers["Content-Type"] = "application/octet-stream";
  }

  if (strcmp(request->method, "POST") != 0) {
      result.statusCode = 405;
      result.error = "Only POST allowed";
      return result;
  }

  if (!request->body) {
      result.statusCode = 400;
      result.error = "Empty body";
      return result;
  }

  std::string body(request->body);
  std::string binaryString;

  try {
      if (contentType.find("application/json") != std::string::npos) {
          json parsed = json::parse(body);

          if (!parsed.contains("value")) {
              result.error = "Missing value field";
              return result;
          }

          binaryString = parsed["value"];
      }
      else if (contentType.find("application/x-www-form-urlencoded") != std::string::npos) {
          const std::string key = "value=";

          if (body.rfind(key, 0) != 0) {
              result.error = "Invalid form format";
              return result;
          }

          binaryString = body.substr(key.length());
      }
      else {
          result.statusCode = 415;
          result.error = "Unsupported Content-Type";
          return result;
      }

      if (binaryString.empty() || binaryString.size() > 8) {
          result.error = "Wrong binary length";
          return result;
      }

      for (char c : binaryString) {
          if (c != '0' && c != '1') {
              result.error = "Invalid binary string";
              return result;
          }
      }

      result.value = std::stoi(binaryString, nullptr, 2);
      result.success = true;
      result.statusCode = 200;
  }
  catch (...) {
      result.statusCode = 400;
      result.error = "Parsing error";
  }

  return result; 
}

void BuildLogicHTTP::SendResponse(struct http_request_s* request, int value) {
  if (value < 0 || value > 255)
      value = 0;

  std::bitset<8> binary(value);

  json response;
  response["value"] = binary.to_string();

  std::string body = response.dump();

  struct http_response_s* res = http_response_init();
  http_response_status(res, 200);
  http_response_header(res, "Content-Type", "application/json");
  http_response_body(res, body.c_str(), body.size());

  http_respond(request, res);
}
