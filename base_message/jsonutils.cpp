#include "jsonutils.h"
#include "cocos2d_server/cocos2d_server.h"
void MJsonPair::add_to(rapidjson::Value &js,rapidjson::Document::AllocatorType& allocator)const{
    rapidjson::Value key(this->key, allocator);
    if(value.type==MJsonValue::INT)
        js.AddMember(key,value.int_value,allocator);

    if(value.type==MJsonValue::BOOL)
        js.AddMember(key,value.bool_value,allocator);

    if(value.type==MJsonValue::LONG){
        rapidjson::Value v;//(value.long_value, allocator);
        v.SetInt64(value.long_value);
        js.AddMember(key,v,allocator);
    }
    if(value.type==MJsonValue::STRING){
        rapidjson::Value v(value.string_value, allocator);
        js.AddMember(key,v,allocator);/**/
    }
    if(value.type==MJsonValue::OBJECT){
         /*rapidjson::Value js=rapidjson::Value(rapidjson::kObjectType);
         for(auto t:value.value ){
             js.AddMember(t.first,t.second,js,allocator);
         }
        js.AddMember(key,v,allocator);/**/
    }
}

string JsonUtil::to_string(const rapidjson::Value &js){
    CCLOG("JsonUtil::to_string");
    rapidjson::StringBuffer strbuf;
    CCLOG("rapidjson::StringBuffer strbuf");
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    CCLOG("rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf)");
    js.Accept(writer);
    CCLOG("js.Accept(writer)");
    auto s=string(strbuf.GetString(), strbuf.GetSize());
    CCLOG("%s",s.c_str());
    return s;/**/
}
rapidjson::Value JsonUtil::create(rapidjson::Document::AllocatorType& allocator,const vector<MJsonPair> &vec){
    rapidjson::Value js=rapidjson::Value(rapidjson::kObjectType);

    for(auto t: vec){
        t.add_to(js,allocator);
    }
    return js;
};
