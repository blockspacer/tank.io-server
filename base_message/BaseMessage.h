#ifndef BASEMESSAGE_H
#define BASEMESSAGE_H
#include <wchar.h>
#define PAK_STRUCT  struct __attribute__ ((packed))
typedef unsigned long long MSG_ID;
typedef long long USER_ID;
typedef long long OBJ_ID;
typedef long long COOCKIE;
typedef long long HASH_KEY;
typedef long long HASH_RESULT;
typedef long long BASE_HASH;
typedef long long ROOM_ACCESS_CODE;
typedef long long TIME_VALUE;
typedef  unsigned int SIZE;

typedef char PASSWORD[32];
typedef char CODE[32];
typedef char REGISTER_CODE[32];
typedef char LOGIN_ERROR[32];
typedef char DISPLAY_NAME[64];
typedef char USE_DISCRIP[64];

PAK_STRUCT BaseMessage{
    enum BaseType:unsigned int{
        ACK=1,
        NCK=2,
        MSG=3,
        PINGR_EQUEST=4,
        PING_RESPONSE=5,
        LOGIN=32|1,
        LOGIN_RESPONSE=32|2,
        LOGIN_RESPONSE_ACK=32|3,
        LOGIN_PROBLEM=32|4,

        PREGISTERREQUEST=32|16|1,
        PREGISTERRESPONSE=32|16|2,
        REGISTER=32|16|3,
        REGISTER_RESPONSE=32|16|4,
        REGISTER_RESPONSE_ACK=32|16|5,
        PYTHON_SERVER_BUY_REQUEST=64|1,
        PYTHON_SERVER_BUY_RESPONSE=64|2,

    }base_type;
    OBJ_ID client_process_id=0;//agr client baste shavad va dobare baz shavad taghir mikonad
    MSG_ID id=0;
    MSG_ID answer_of_msg_id=0;
    USER_ID user_id;
    COOCKIE cookie;
    HASH_KEY rand_key;
    BASE_HASH hash;
    SIZE size_of_data;
    SIZE start_of_body;
    bool ack_required=true;
    BaseMessage(){}
    BaseMessage(BaseType _type):base_type(_type){}
    BaseMessage(BaseType _type,MSG_ID _id):base_type(_type),id(_id){}
    inline bool check(){
        return true;
    }
    char * getEndData(){
        return (char*)this+size_of_data;
    }
};
PAK_STRUCT ACKMSG:public BaseMessage{
       int ack_lvl;
       ACKMSG():BaseMessage(ACK){
           ack_required=false;
       }

};


PAK_STRUCT PreRegisterRequest:public BaseMessage{
   REGISTER_CODE code;
   PreRegisterRequest():BaseMessage(PREGISTERREQUEST){}
};

PAK_STRUCT PreRegisterResponse:public BaseMessage{
  REGISTER_CODE code;
  long long id_and_public_key;
  long long hash_result;
  PreRegisterResponse():BaseMessage(PREGISTERRESPONSE){}
};

PAK_STRUCT Register:public BaseMessage{
    REGISTER_CODE code;
    long long id_and_public_key;
    long long hash_result;
    long long private_key;
    Register():BaseMessage(REGISTER){}
};

PAK_STRUCT RegisterResponse:public BaseMessage{
    bool done;
    PASSWORD pass;
    COOCKIE cookie;
    RegisterResponse():BaseMessage(REGISTER_RESPONSE){}
};


PAK_STRUCT LoginMSG:public BaseMessage{
    PASSWORD password;
    LoginMSG():BaseMessage(LOGIN){}
};


PAK_STRUCT LoginResponse:public BaseMessage{
    bool done;
    //LOGIN_ERROR error;
    //OBJ_ID auto_join_to_room_id; // if zero not join to room
    DISPLAY_NAME display_name;
    LoginResponse():BaseMessage(LOGIN_RESPONSE){}
};


PAK_STRUCT LoginProblem:public BaseMessage{
    enum PROBLEM:unsigned int{
        CREATE_NEW_TCP=1,
        RETRY_LOGIN=2,
    }problem;
    LoginProblem():BaseMessage(LOGIN_PROBLEM){}
};


PAK_STRUCT PingRequestMsg:public BaseMessage{
    int version_id;
    int branch_id;
    TIME_VALUE pinger_time;
    PingRequestMsg():BaseMessage(BaseType::PINGR_EQUEST){
        ack_required=false;

    }

};
PAK_STRUCT PingResponseMsg:public BaseMessage{
    TIME_VALUE pinger_time;
    TIME_VALUE responser_time;
    bool upgrade_requrid=false;
    PingResponseMsg():BaseMessage(BaseType::PING_RESPONSE){
        ack_required=false;
    }

};
#endif // BASEMESSAGE_H
