#ifndef METAMESSAGES_H
#define METAMESSAGES_H
#include <string>
#include "BaseMessage.h"
#include "jsonutils.h"
using namespace std;
/*struct PreRegisterRequest:public BaseMessage{
   REGISTER_CODE code;
   PreRegisterRequest():BaseMessage(PREGISTERREQUEST){}
};

struct PreRegisterResponse:public BaseMessage{
  REGISTER_CODE code;
  long long id_and_public_key;
  long long hash_result;
  PreRegisterResponse():BaseMessage(PREGISTERRESPONSE){}
};/**/

struct RegisterRequest{
    std::string code;
    rapidjson::Value getJson(rapidjson::Document::AllocatorType& allocator){
        return JsonUtil::create(allocator,{{"code",code}});
    }
};

struct RegisterResponse{
    bool done;
    COOCKIE cookie;
    USER_ID userId;
    string pass;
    rapidjson::Value getJson(rapidjson::Document::AllocatorType& allocator){
        return JsonUtil::create(allocator,{{"done",done},
                                 {"userId",userId},
                                 {"cookie",cookie},
                                 {"pass",pass}
                                });
    }
    void loadFromJson(rapidjson::Value &js){
        done=js["done"].GetBool();
        userId=js["userId"].GetInt64();
        cookie=js["cookie"].GetInt64();
        pass=js["pass"].GetString();
    }
};
#endif // METAMESSAGES_H
