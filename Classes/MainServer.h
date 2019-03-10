#ifndef MAINSERVER_H
#define MAINSERVER_H
#include "base_message/BaseMessage.h"
#include "Engine/BaseServer.h"
#include "game_message/societalgaming.h"
#include "Engine/SharedMemoryLoger.h"



struct MyUserPort:public UserPort{

};
struct MainServer;




struct MainServer;
struct ChangeNameJob:public DBJob{
    ChangeNameJob(MainServer *s):DBJob((BaseServer*)s){}
    ChangeNameRequest change_name_request;

    void do_job();

    void init(UserPort *user_port);
};

struct SaveUserData:public DBJob{
    SaveUserData(MainServer *s):DBJob((BaseServer*)s){}
    //MainServer *server;
    USER_ID user_id;
    void do_job();
    void init(USER_ID user_id);
};
struct SaveUseMony:public DBJob{
    SaveUseMony(MainServer *s):DBJob((BaseServer*)s){}
    //MainServer *server;
    USER_ID user_id;
    int mony;
    USE_DISCRIP dis;
    void do_job();
    void init(USER_ID user_id,int ,const USE_DISCRIP);
};


struct LoadExtraData:public DBJob{
    LoadExtraData(MainServer *s):DBJob((BaseServer*)s){}
    //MainServer *server;
    USER_ID user_id;
    void do_job();
    void init(USER_ID user_id);
};
struct SaveTeamLevel:public DBJob{
    SaveTeamLevel(MainServer *s):DBJob((BaseServer*)s){}
    //MainServer *server;
    USER_ID user_id;
    int team_id;
    void do_job();
    void init(USER_ID user_id,int team_id);
};

struct SaveConfigs:public DBJob{
    SaveConfigs(MainServer *s):DBJob((BaseServer*)s){}
    //MainServer *server;
    USER_ID user_id;
    int postion_id;
    void do_job();
    void init(USER_ID user_id,int postion_id);
};


struct SaveRoom:public DBJob{
    SaveRoom(MainServer *s):DBJob((BaseServer*)s){}
    //MainServer *server;
    Room *room;
    void do_job();
    void init(Room  *room);
};

class MainServer:public BaseServer{
public:
    int game_count=10;
    USER_ID ligs[2][16];
    TIME_VALUE ligs_time[2][16];
    int team_price[128];
    map<OBJ_ID , std::shared_ptr<Room> > rooms;
    map<int,vector<OBJ_ID> > rooms_lig;
    MainServer(boost::asio::io_service& io_service, short port):BaseServer(io_service,port){


    }
    void first_udp_ping(UserPort *u);
    void sticker(UserPort *s,StikerMsg *mr);
    void small_chat(UserPort *s,SmallGameChat *mr);
    void room_msg(UserPort *s,RoomMsg *mr);
    void update_client_profile(USER_ID user_id);
    void update_client_game_data(USER_ID user_id);
    void server_data_request(UserPort *s,ServerDataRequest *msg);
    void top_hundred_request(UserPort *s,TopHundredRequest *msg);
    void hundred_state_request(UserPort *s,HundredStateRequest *msg);
    void update_user_db(USER_ID user_id );
    void game_room_msg(UserPort *s,GameActionMsg *mr);
    void close_game(UserPort *s,CloseGame *mr );


    void change_dispaly_name(UserPort *s,ChangeNameRequest *mr);
    void change_team(UserPort *s,ChangeTeamRequest *mr);

    void buy_postion(UserPort *s,BuyPostion *mr);

    void use_mony(UserPort *s,UseMony *mr);

    void buge_repoart(UserPort*s,BugCheckPoint *msg);
    void game_request(UserPort *s,GameRequest *msg);
    void game_request_response(UserPort *s,GameRequestResponse *msg);
    Room *create_room(int lig_id);
    bool check_user_in_room(UserPort *s,UserPort *s2);

    void update_room_client(Room *room,UserPort *up);
    void update_client(Room *room);
    void profile_request(UserPort *s,RequestProfile *mr);


    void match_request(UserPort *s,JoinRequest *mr);
    void room_event_ack(UserPort *s, RoomEventAck *mr);

    void run_rooms();
    void run_rooms_while();
    void run_rooms_step();


    int achivment_gem[100]={0,1,2,1,2,1,2,2,4,2,4,2,4,10,20,10,20,10,20,10,20,10,20,10,20};
    int lig_exp[100]={0,10,20,25,30,35,40,45,50,60,65,70,75,80,85,90,100};
    int lvl_exp[300];
    int lvl_gem[300];
    void add_achivment(USER_ID user_id,int achivment);
    void lig_win(USER_ID user_id,int lig_id,int coin_winer);

    ResponseProfile* get_user_profile_msg(USER_ID );

    ChangeNameResponse change_display_name(UserData *ud);
    void save_user_data(USER_ID s);
    void save_use_mony(USER_ID uid,int mony,std::string dis);

    void recive_msg(UserPort *s, BaseMessage *msg,size_t );
    void recive_tcp_msg(UserPort *s, BaseMessage *msg);
    void change_user_data(USER_ID);
    void new_login(USER_ID user_id,UserPort *user_port);
    void after_register(UserPort *s,USER_ID user_id);

    void reload_rooms();
};

#endif // MAINSERVER_H
