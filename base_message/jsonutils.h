#ifndef JSON_H
#define JSON_H
#define RAPIDJSON_HAS_STDSTRING 1
#include <string>
#include <map>
#include <vector>
#if SERVER_SIDE
    #include "rapidjson/document.h"
    #include "rapidjson/writer.h"
    #include "rapidjson/stringbuffer.h"
#else
    #include "../external/json/writer.h"
    #include "../external/json/stringbuffer.h"
    #include "../external/json/rapidjson.h"
    #include "../external/json/document.h"
    #include "../external/json/filereadstream.h"
    #include "../external/json/filewritestream.h"
#endif
using namespace std;
struct MJsonPair{
    struct MJsonValue{
        enum Type{
            NONE=0,
            INT=1,
            LONG=2,
            STRING=3,
            OBJECT=4,
            BOOL=5,
        }type;
        union{
            int int_value;
            long long long_value;
            bool bool_value;
        };
        string string_value;
        map<string,MJsonValue*> value;
        MJsonValue(int x){
            type=Type::INT;
            int_value=x;
        }
        MJsonValue(long long x){
            type=Type::LONG;
            long_value=x;
        }
        MJsonValue(bool x){
            type=Type::BOOL;
            bool_value=x;
        }
        MJsonValue(string x){
            type=Type::STRING;
            string_value=x;
        }
        MJsonValue(char x[]){
            type=Type::STRING;
            string_value=string(x);
        }
        MJsonValue(const vector<MJsonPair> &vec){

        }

    };
    string key;
    MJsonValue value;
    void add_to(rapidjson::Value &js,rapidjson::Document::AllocatorType& allocator)const;
};
struct JsonUtil{
    rapidjson::Document jsonDoc;
    rapidjson::Value js=rapidjson::Value(rapidjson::kObjectType);
    rapidjson::Document::AllocatorType *allocator;
    static JsonUtil create(const vector<MJsonPair> &vec);
    string to_string()const;
};
#endif // JSON_H
