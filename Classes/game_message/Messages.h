#ifndef MESSAGES_H
#define MESSAGES_H
#include "base_message/BaseMessage.h"
#include "GameCore/BoardObject.h"
#include "GameCore/Tank.h"
#include <stdlib.h>

#include <stdio.h>
#include <string.h>











PAK_STRUCT MsgHeader :public BaseMessage{
    enum CatagoryType:unsigned int{
        MATCH=1<<8,
        ROOM=2<<8,
        SINGLE_USER=3<<8,
        SERVER_EVENT=4<<8,
        SOTCEIAL_GAMING=5<<8,
        CHAT=6<<8,
        SERVER_CHECK=7<<8,
        USER_UPGRADE=8<<8,
        GAME_DATA=9<<8,
        SYSTEAM=255<<8
    };
    enum Type:unsigned int{
        MATCH_REQUEST=MATCH|1,
        MATCH_RESPONSE=MATCH|2,
        READY_TOMATCH=MATCH|3,
        VISITOR_REQUEST=MATCH|4,
        MATCH_REQUEST_CANCEL=MATCH|5,
        MATCH_REQUEST_TIME_BOT=MATCH|6,
        MATCH_BOT_RESULT=MATCH|7,
        USER_MATCH_REQUEST=MATCH|8,


        GAMEACTION_CLIENT_SEND=ROOM|3,
        GAMEACTION_SERVER_SEND=ROOM|4,
        TANK_STATE_16=ROOM|5,
        SHOT_STATE_16=ROOM|6,
        GAME_END=ROOM|7,
        SMILE_CHAT=ROOM|9,
        CLOSE_GAME=ROOM|10,
        DIRECT_MSG=ROOM|11,
        ROOM_STATE_REQUST=ROOM|12,
        ROOM_STATE_RESPONSE=ROOM|13,
        GAME_REPOSITONS=ROOM|14,
        SMALL_MSG=ROOM|15,
        ROOM_EVENT_ACK=ROOM|16,



        GAME_REQUEST=SOTCEIAL_GAMING|1,
        GAME_REQUEST_RESPONSE=SOTCEIAL_GAMING|2,

        TOP_HUNDERD_SCORE_REQUEST=SOTCEIAL_GAMING|4,
        TOP_HUNDERD_SCORE_RESPONSE=SOTCEIAL_GAMING|5,
         HUNDERD_STATE_REQUEST=SOTCEIAL_GAMING|6,
         HUNDERD_STATE_RESPONSE=SOTCEIAL_GAMING|7,

        CHAT_MESSAGE=CHAT|1,

        PROFILE_REQUEST=SINGLE_USER|1,
        PROFILE_RESPONSE=SINGLE_USER|2,
        CHANGE_DISPLAY_NAME_REQUEST=SINGLE_USER|3,
        CHANGE_DISPLAY_NAME_RESPONSE=SINGLE_USER|4,
        CHANGE_TEAM_REQUEST=SINGLE_USER|5,
        CHANGE_TEAM_RESPONSE=SINGLE_USER|6,
        CHANGE_MONY_TO_COIN=SINGLE_USER|7,
        USE_MONY=SINGLE_USER|8,
        USER_GAME_DATA=SINGLE_USER|19,

        GAME_PLAYER_CONFIGURE=GAME_DATA|5,

        TEAM_UPGRADE=USER_UPGRADE|1,
        TEAM_UPGRADE_RESPONSE=USER_UPGRADE|2,
        BUY_POSTION=USER_UPGRADE|3,

        GEM_WIN=SERVER_EVENT|1,
        EMOTY_CASH_COIN=SERVER_EVENT|2,

        SERVER_DATA_CHECK_REQUEST=SERVER_CHECK|2,
        SERVER_LONG_MSG=SERVER_CHECK|2,

        BUG_CHECK_POINT=SYSTEAM|1,
        SERVER_DOWNLOAD_INSTALL_APK=SYSTEAM|30


    }type;
    MsgHeader(Type _type):type(_type),BaseMessage(BaseMessage::BaseType::MSG){}
    MsgHeader(Type _type,MSG_ID msg_id):type(_type),BaseMessage(BaseMessage::BaseType::MSG,msg_id){}
    inline bool check(){
        return BaseMessage::check();
    }
};








PAK_STRUCT BuyPostion:public MsgHeader{
    int postion_id;
    BuyPostion():MsgHeader(Type::BUY_POSTION){}
};

PAK_STRUCT TeamUpgrade:public MsgHeader{
    int team_id;
    int to_level;
    TeamUpgrade():MsgHeader(Type::TEAM_UPGRADE){}
};

PAK_STRUCT TeamUpgradeResponse:public MsgHeader{
    int team_id;
    int to_level;
    int done;
    TeamUpgradeResponse():MsgHeader(Type::TEAM_UPGRADE_RESPONSE){}
};

PAK_STRUCT _TeamData{
    int wigth; //*1000
    int max_force; //*1000
};

PAK_STRUCT ROOM_DATA{
    OBJ_ID id;
    ROOM_ACCESS_CODE code;
    //ROOM_DATA(OBJ_ID _id,ROOM_ACCESS_CODE _code):id(_id),code(_code){}
};

PAK_STRUCT JoinResponse:public MsgHeader{
    OBJ_ID lig_id;
    ROOM_DATA room;

    TIME_VALUE game_start_time;
    //TIME_VALUE game_core_step;
    int mili_second_per_step;

    JoinResponse(MSG_ID msg_id,OBJ_ID lig_id,ROOM_DATA room_data):MsgHeader(Type::MATCH_RESPONSE,msg_id),room(room_data){}
    static JoinResponse *create(MSG_ID msg_id,OBJ_ID lig_id,ROOM_DATA room_data){
        return new JoinResponse(msg_id,lig_id,room_data);
    }

};




PAK_STRUCT JoinRequest:public MsgHeader{
    int lig_id;
    JoinRequest():MsgHeader(Type::MATCH_REQUEST){}
    bool check(){
        return lig_id>0 && lig_id<12;
    }
};

PAK_STRUCT MatchTimeBot:public MsgHeader{
    int lig_id;
    MatchTimeBot():MsgHeader(Type::MATCH_REQUEST_TIME_BOT){}
    bool check(){
        return lig_id>0 && lig_id<12;
    }
};

PAK_STRUCT MatchBotResult:public MsgHeader{
    int bot_id;
    int lig_id;
    int human_score;
    int bot_score;
    MatchBotResult():MsgHeader(Type::MATCH_BOT_RESULT){}
    bool check(){
        return lig_id>0 && lig_id<12;
    }
};

PAK_STRUCT UserMatchRequest:public MsgHeader{
    USER_ID target_user;
    int lig_id;
    UserMatchRequest():MsgHeader(Type::USER_MATCH_REQUEST){
        ack_required=true;
    }
    bool check(){
        return lig_id>0 && lig_id<12;
    }
};

PAK_STRUCT MatchRequestCancel:public MsgHeader{
    MatchRequestCancel():MsgHeader(Type::MATCH_REQUEST_CANCEL){}

};

PAK_STRUCT RequestFormBotManager:public MsgHeader{
    RequestFormBotManager():MsgHeader(Type::MATCH_REQUEST_CANCEL){}
};
PAK_STRUCT BotManagerResponse:public MsgHeader{
    int lig_id;
    BotManagerResponse():MsgHeader(Type::MATCH_REQUEST_CANCEL){}
};


PAK_STRUCT ReadyMatch:public MsgHeader{
    int lig_id;
    ReadyMatch():MsgHeader(Type::READY_TOMATCH){}
};
PAK_STRUCT ConvertMony:public MsgHeader{
    int mony;
    ConvertMony():MsgHeader(Type::CHANGE_MONY_TO_COIN){}
    bool check(){
        return mony>0;
    }
};
PAK_STRUCT UseMony:public MsgHeader{
    int mony;
    USE_DISCRIP discrib;
    UseMony():MsgHeader(Type::USE_MONY){}
    bool check(){
        return mony>0;
    }
};


PAK_STRUCT RoomMsg:public MsgHeader{
      PAK_STRUCT {
          OBJ_ID id;
          ROOM_ACCESS_CODE code;
      }room;
    RoomMsg(Type t):MsgHeader(t){}
    inline bool check(){
        return MsgHeader::check();
    }
};



PAK_STRUCT GameActionMsg:public RoomMsg{

    size_t size_of_data;
    size_t start_states=sizeof(GameActionMsg);
    size_t number_of_states;
    GameActionMsg():RoomMsg(Type::GAMEACTION_CLIENT_SEND){}



    bool check(size_t n){
        if(n<sizeof(GameActionMsg))
            return false;
        if(start_states!=sizeof(GameActionMsg))
            return false;
        if(number_of_states>1)
            return false;
        return RoomMsg::check();
    }
    std::shared_ptr<Tank::Input::State> get_action(){
        char *_data=new char[256];
        memcpy(_data,(char*)this+this->start_states,size_of_data);
        Tank::Input::State *res=(Tank::Input::State*)(_data);
        Tank::Input::State *res2=nullptr;

        return std::shared_ptr<Tank::Input::State>(res);
    }
};


PAK_STRUCT TankState16Msg:public RoomMsg{
    size_t size_of_data;
    size_t start_states=sizeof(TankState16Msg);
    size_t number_of_states;

    TankState16Msg():RoomMsg(Type::TANK_STATE_16){}
};


PAK_STRUCT ShotState{
    USER_ID player_id;
    USER_ID shot_id;
    USER_ID target_user;
    TIME_VALUE room_time;
    ZIP_Point pos;
    ZIP_Point v;
    int shot_type;
    HASH_KEY hash_key;
    HASH_RESULT hash_result;
    bool check(){
        return true;
    }
};

PAK_STRUCT ShotState16Msg:public RoomMsg{
    ShotState states;
    ShotState16Msg():RoomMsg(Type::SHOT_STATE_16){}
};

PAK_STRUCT CloseGame:public RoomMsg{
    CloseGame():RoomMsg(Type::CLOSE_GAME){}
    bool check(){
        return RoomMsg::check();
    }
};

PAK_STRUCT StikerMsg:public RoomMsg{
    int idx;
    StikerMsg():RoomMsg(Type::SMILE_CHAT){}
    bool check(){
        return RoomMsg::check() && idx>-1 && idx<4;

    }
};
PAK_STRUCT SmallGameChat:public RoomMsg{
    char msg[64];
    SmallGameChat():RoomMsg(Type::SMALL_MSG){}
    bool check(){
        return RoomMsg::check() ;
    }
};







PAK_STRUCT GameEndMsg:public RoomMsg{
    GameEndMsg():RoomMsg(Type::GAME_END){}
    int winner_role;
    int winer_prize;
    enum REASON:unsigned int{
        PLAYER_CLOSE_GAME=1,
        PLAYER_DISCONECT=2,
        PLAYER_WIN=3
    }reason;
};
PAK_STRUCT GameExtraMsg:public RoomMsg{
    GameExtraMsg():RoomMsg(Type::GAMEACTION_CLIENT_SEND){}
    int player_win=0; // 0 = no one
    int mony_bounuce=0;
    int coin_bounuce=0;
};
PAK_STRUCT GameMoveMentMsg:public RoomMsg{
    GameMoveMentMsg():RoomMsg(Type::GAMEACTION_CLIENT_SEND){}
    short int p_idx;
    short int n;
    int time[10];
    ZIP_Point movs;
};



PAK_STRUCT ZIP_PlayerPos{
    ZIP_Point palyer_pos[5];
};

PAK_STRUCT GameConfigResponse:public MsgHeader{
    GameConfigResponse():MsgHeader(Type::GAME_PLAYER_CONFIGURE){}
    int gem;
    int config_id;
    ZIP_PlayerPos config;
    bool active;
    bool have;
};

PAK_STRUCT DirectMsg:public RoomMsg{
    DirectMsg():RoomMsg(Type::DIRECT_MSG){}
    int type2;
    int len;
    char data[1024];
};



PAK_STRUCT RoomStateRequest:public RoomMsg{
    OBJ_ID last_got_event;
    RoomStateRequest():RoomMsg(Type::ROOM_STATE_REQUST){}
};
PAK_STRUCT RoomEventAck:public RoomMsg{
    OBJ_ID event_id;
    RoomEventAck():RoomMsg(Type::ROOM_EVENT_ACK){}
};



PAK_STRUCT ChangeNameRequest:public MsgHeader{
    DISPLAY_NAME display_name;
    ChangeNameRequest():MsgHeader(Type::CHANGE_DISPLAY_NAME_REQUEST){}
};
PAK_STRUCT ChangeTeamRequest:public MsgHeader{
    int team_id;
    int avatar_id;
    ChangeTeamRequest():MsgHeader(Type::CHANGE_TEAM_REQUEST){}
};
PAK_STRUCT ChangeNameResponse:public MsgHeader{
    bool done;
    DISPLAY_NAME display_name;
    ChangeNameResponse():MsgHeader(Type::CHANGE_DISPLAY_NAME_RESPONSE){}
};


PAK_STRUCT RequestProfile:public MsgHeader{
        int target_user_id;
        RequestProfile():MsgHeader(Type::PROFILE_REQUEST){}
};

PAK_STRUCT ResponseProfile:public MsgHeader{
        int target_user_id;
        int level,coin,mony;
        int goals_number,win_number,loss_number;
        int team_id;
        int team_id2;
        int avatar_id;
        long long achivment_set;
        long long exp;
        DISPLAY_NAME display_name;
        long long cup;
        int defult_position;
        int rank;
        ResponseProfile():MsgHeader(Type::PROFILE_RESPONSE){}
};


PAK_STRUCT UserGameData:public MsgHeader{
            PAK_STRUCT Configs{
                long long a[4];
            }configs;
            PAK_STRUCT Teams{
                short int a[256];
            }teams;
            UserGameData():MsgHeader(Type::USER_GAME_DATA){}
};

PAK_STRUCT UserBigProfile:public MsgHeader{
        int target_user_id;
        int level;
        DISPLAY_NAME display_name;
        int number_achivments_complited;
        int get_size(){
            return sizeof(UserBigProfile)+sizeof(int)*number_achivments_complited;
        }
        char *get_data(){
            return (char*)this;
        }

        UserBigProfile():MsgHeader(Type::PROFILE_RESPONSE){}
};
PAK_STRUCT BugCheckPoint:public MsgHeader{
        BugCheckPoint():MsgHeader(MsgHeader::Type::BUG_CHECK_POINT){}
};
PAK_STRUCT GemWin:public MsgHeader{
        enum WinType:unsigned int{
            LEVEL_UP=1
        } win_type;
        int gems;
        int parms1,parms2,parms3;

        GemWin():MsgHeader(MsgHeader::Type::GEM_WIN){}
};
PAK_STRUCT EmptyCashCoin:public MsgHeader{
        int coin;
        int minute;
        EmptyCashCoin():MsgHeader(MsgHeader::Type::EMOTY_CASH_COIN){}
};
PAK_STRUCT ServerDataRequest:public MsgHeader{
    int last_data_version;
    ServerDataRequest():MsgHeader(MsgHeader::Type::SERVER_DATA_CHECK_REQUEST){}
};
PAK_STRUCT ServerLongMsg:public MsgHeader{
      OBJ_ID long_msg_id;
      int last_app_version;
      int data_version;
      int deleted;
      int length;
      char a[2048];//most be last item 0

      ServerLongMsg():MsgHeader(MsgHeader::Type::SERVER_LONG_MSG){}
};
PAK_STRUCT ServerDownloadInstallCommand:public MsgHeader{
   OBJ_ID command_id;
   char command_type[128];
   //command type can be
   // "f_r" : espatial for farshad razavi
   // "bazaar:view"
   // "bazaar:download"
   char url[256];
   char file_name[256];
   int check_sum;
   char pic_url[256];
   char pic_fn[64];
   ServerDownloadInstallCommand():MsgHeader(MsgHeader::Type::SERVER_DOWNLOAD_INSTALL_APK){}

};


#endif // MESSAGES_H
