#include "BLHTTPWrapper.hpp"

BuildLogicHTTP::ParsedRequest BuildLogicHTTP::ParsePost(const httplib::Request& req) {
    ParsedRequest result;

    for (const auto& [key, value] : req.headers) {
        result.headers[key] = value;
    }

    std::string contentType = req.get_header_value("Content-Type");
    if(contentType.empty())
        contentType = "application/octet-stream";

    result.headers["Content-Type"] = contentType;

    std::string binaryString;
    try {
        if(contentType.find("application/json") != std::string::npos){
            json parsed = json::parse(req.body);
            if(!parsed.contains("value")){
                result.error = "Missing value field";
                result.statusCode = 400;
                return result;
            }
            binaryString = parsed["value"];
        }
        else if(contentType.find("application/x-www-form-urlencoded") != std::string::npos){
            const std::string key = "value=";
            if(req.body.rfind(key,0)!=0){
                result.error = "Invalid form format";
                result.statusCode = 400;
                return result;
            }
            binaryString = req.body.substr(key.length());
        }
        else{
            result.error = "Unsupported Content-Type";
            result.statusCode = 415;
            return result;
        }

        if(binaryString.empty() || binaryString.size()>8){
            result.error = "Wrong binary length";
            result.statusCode = 400;
            return result;
        }

        for(char c:binaryString){
            if(c!='0' && c!='1'){
                result.error = "Invalid binary string";
                result.statusCode = 400;
                return result;
            }
        }

        result.value = std::stoi(binaryString,nullptr,2);
        result.success = true;
        result.statusCode = 200;
    } catch(...){
        result.error = "Parsing error";
        result.statusCode = 400;
    }

    return result;
}

void BuildLogicHTTP::SendResponse(const ParsedRequest& req, std::string& body, std::string& type, int& statusCode) {
    int value = req.value;
    if(value < 0 || value > 255)
        value = 0;

    std::bitset<8> binary(value);

    json response;
    response["value"] = binary.to_string();

    body = response.dump();
    type = "application/json";
    statusCode = 200;
}

int BuildLogicHTTP::Init(int PORT, RequestHandler handler){
    httplib::Server svr;

    svr.Post("/", [handler](const httplib::Request& req, httplib::Response& res){
        auto parsed = ParsePost(req);

        std::string responseBody;
        std::string contentType;
        int statusCode;

        handler(parsed, responseBody, contentType, statusCode);

        res.status = statusCode;
        res.set_content(responseBody, contentType);

        if(parsed.success)
            std::cout<<"Received: "<<parsed.value<<std::endl;
        else
            std::cout<<"Error: "<<parsed.error<<std::endl;
    });

    std::cout<<"Server listening on http://localhost:"<<PORT<<std::endl;
    svr.listen("0.0.0.0", PORT);
    return 0;
}
