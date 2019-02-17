#ifndef SOCIETALGAMING_H
#define SOCIETALGAMING_H


#include "Messages.h"



PAK_STRUCT GameRequest:public MsgHeader{
   OBJ_ID request_id;
   USER_ID target_user_id;
   int lig_id;

   //int level;
   //DISPLAY_NAME display_name;
    GameRequest():MsgHeader(MsgHeader::Type::GAME_REQUEST){}
};
PAK_STRUCT GameRequestResponse:public MsgHeader{
  OBJ_ID request_id;
  USER_ID target_user_id;
  int lig_id;

  bool is_accepted=false;
  enum RejectReason:int{
        PLAYER_IS_OFLINE=1,
        PLAYER_NOT_HAVE_COIN=2,
        PLAYER_REJECTED=3,
        FAST_REQUEST=4,
        REQUEST_CANCEL=5
  }reject_reason;
  GameRequestResponse():MsgHeader(MsgHeader::Type::GAME_REQUEST_RESPONSE){}
};
PAK_STRUCT AddFreind:public MsgHeader{
  USER_ID target_usr_id;
  COOCKIE code;
};
PAK_STRUCT HundredStateRequest:public MsgHeader{
  int n; //size of a
  USER_ID a[128];
  HundredStateRequest():MsgHeader(MsgHeader::Type::HUNDERD_STATE_REQUEST){}
};
PAK_STRUCT HundredStateResponse:public MsgHeader{
  int n;
  PAK_STRUCT Profile{
      int id;
      int rank;
      int cup;
      int avatar_id;
      int team_id;
      int level;
      int city_id;
      DISPLAY_NAME name;
      enum State:unsigned int{
          NOT_USER=0,
          OFLINE=1,
          ONLINE=2,
          IN_GAME=3
      }state;
  }profiles[100];
  HundredStateResponse():MsgHeader(MsgHeader::Type::HUNDERD_STATE_RESPONSE){}
};

PAK_STRUCT TopHundredRequest:public MsgHeader{
    OBJ_ID section_id;
    int shift;
    int count;
    TopHundredRequest():MsgHeader(MsgHeader::Type::TOP_HUNDERD_SCORE_REQUEST){}
    bool check(){
        return count<=100;
    }
};
PAK_STRUCT TopHundredResponse:public MsgHeader{
    OBJ_ID section_id;
    int shift;
    int count;
    USER_ID profiles[100];
    TopHundredResponse():MsgHeader(MsgHeader::Type::TOP_HUNDERD_SCORE_RESPONSE){}
};



PAK_STRUCT ChatMessage:public RoomMsg{
private:
  ChatMessage():RoomMsg(MsgHeader::Type::CHAT_MESSAGE){}
public:
      USER_ID target_conversation_id;
      OBJ_ID msg_id;// uniq on server for all users; hich to msg i dar id  ba yekdigar barabar nistand
      int length;
      char a[1027];//most be last item

      const inline int get_length()const{
          return sizeof(ChatMessage)+length;
      }

      const char* get_data()const{
          return a;
      }
      void init(int length){
          //MsgHeader::MsgHeader(MsgHeader::Type::EMOTY_CASH_COIN);
          base_type=BaseMessage::BaseType::MSG;
          type=MsgHeader::Type::EMOTY_CASH_COIN;
          this->length=length;
      }

      static ChatMessage *create(int length){
          char *s=new char[sizeof(ChatMessage)+length];
          ChatMessage *msg=(ChatMessage*)msg;
          msg->init(length);
          return msg;
      }
};


PAK_STRUCT GetOfileneMsg:public MsgHeader{

};




#endif // SOCIETALGAMING_H

