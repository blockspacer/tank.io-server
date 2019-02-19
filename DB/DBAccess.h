#ifndef DBACCESS_H
#define DBACCESS_H
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <set>
#include <hash_map>
#include <map>
#include <unordered_map>
#include "game_message/Messages.h"
#include "base_message/BaseMessage.h"

//#include <qt5/QtSql/QSqlDatabase>



#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QVariant>


#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
/*#include "mysql_connection.h"
#include "mysql_driver.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
/**/
#include "base/rbtree.h"


//#define EXAMPLE_DB "globulin"

using namespace std;

struct UserEntity{
    long long user_id;
    PASSWORD pass;
};
struct UserExtraData;
struct UserData{
    USER_ID id;
    int level,coin,exp;
    int lose_number,win_number;
    long long total_coin_winer;
    int avatar_id=0;
    int app_version;
    bool changed=false;
    enum STATE{
        NOT_EXIST=0,
        ACTIVE=1,
        DEACTIVE=2,
        BAN=3
    }state=NOT_EXIST;
    DISPLAY_NAME display_name;
    PASSWORD pass;
    void *port=nullptr;
    TIME_VALUE free_coin_time=0;
    int city_id;
    int cup_on_set_not_updated;
    int name_changed=0;
    UserExtraData *extra_data;
    void set_app_version(int a);
    void close_extra();
};

struct UserExtraData{
    int team_level[64];
    bool postions[128];
    set<OBJ_ID> chat_rooms;
    set<USER_ID> freinds;
    set<USER_ID> blocks;
    UserExtraData(){
        memset(team_level,0,sizeof(team_level));
        memset(postions,0,sizeof(postions));
        postions[0]=postions[1]=1;
        team_level[0]=team_level[1]=team_level[2]=1;
    }
};


struct BaseServer;

struct DBAccess;
struct ScoreIndex{
    int score;
    USER_ID user_id;
    bool operator <(const ScoreIndex &l)const;
};

struct SortedList{
    DBAccess *db;
    RBTree<int > cupssdf;
    RBTree<ScoreIndex > cups_users;//TODO
    void remove_user(USER_ID user_id);
    void change_user(USER_ID user_id);
};
struct Room;
class DBAccess
{
    map<long long,std::shared_ptr<UserData>> users;
    int max_user_id=0;
    BaseServer *server;
    std::string EXAMPLE_USER ="root";
    std::string EXAMPLE_PASS ="ok1";
    SortedList sorted_list;

public:
    DBAccess(BaseServer *ser);
    void connect();
    bool login(USER_ID user_id,PASSWORD pass,UserData **display_name);
    int fake_register_id=0;
    RegisterResponse *register_user(const REGISTER_CODE code,std::string);
    bool change_display_name(UserData *ud);
    bool save_user_data(USER_ID user_id);
    void change_score(USER_ID);
    int get_top_hundred(UserData *user_data,int shift,int count,int &rank,USER_ID users[]);
    int get_user_rank(UserData*);
    void load_game_data();
    bool save_app_version(USER_ID user_id);

    bool save_use_mony(USER_ID uid, int mony, string dis);

    void bug_check_point(USER_ID user_id);
    void add_user_mony(USER_ID,unsigned long long mony);
    void set_buy_item_used(int db_id);


    UserData *get_user_data_by_id(USER_ID user_id);
    void load_extra(USER_ID user_id);
    void save_team_levl(USER_ID user_id,int team_id);
    void save_postions(USER_ID user_id,int postion_id);
    void save_room(Room *room);
    void load_extra_if_not_exist(USER_ID user_id);

};

#endif // DBACCESS_H
