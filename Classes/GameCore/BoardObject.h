#ifndef BOARDOBJECT_H
#define BOARDOBJECT_H
#include "cocos2d_server/cocos2d_server.h"
#include <map>
typedef long long USER_ID;
typedef long long OBJ_ID;
typedef long long TIME_VALUE;
typedef int STEP_VALUE;
//#define PAK_STRUCT  struct __attribute__ ((packed))
#define PAK_STRUCT  struct
struct BoardObject;
struct GameCore;



PAK_STRUCT Decimal{
    int argham,nemad;
};

PAK_STRUCT ZIP_Point{
    float x,y;
    ZIP_Point():x(0),y(0){}
    ZIP_Point(float _x,float _y):x(_x),y(_y){}
    static ZIP_Point fcreate(float x,float y){
        return ZIP_Point(x,y);
    }
    static ZIP_Point fcreate(const Point &p){
        return ZIP_Point(p.x,p.y);
    }
    bool operator!=(const ZIP_Point &r)const {
        if(abs(x-r.x)>2)
            return true;
        return abs(y-r.y)>2;
    }
    ZIP_Point(Point v){
        x=v.x;
        y=v.y;
    }
    Point get()const{
        return Point(x,y);
    }
};
PAK_STRUCT BoardObjectState{
    enum ObjectType{
        TANK=1,
        MISLE=2,
        CircleBlock=3,
        LineBlock=4
    }object_type;
    enum EventType{
      ADD=1,
      DIE=2,
      CROSS=3,//die ro natije mide
      REMOVE=4
    }event_type;
    OBJ_ID object_id;
    OBJ_ID owner_id=0;
    STEP_VALUE room_time;
    ZIP_Point pos;
    float z;
    float r=0;
    float angle;

    bool check(){
        return true;
    }


    static void show(int t[] ,int len){
        string str="";
        for(int i=0; i<len; ++i)
            str+=std::to_string(t[i])+":";
        CCLOG("%s",str.c_str());

    }
};


struct ViewShadow;

struct PostionConfig{
    Point pos;
    float z;
    float angle;
};
struct PostionConfigTime:PostionConfig{
    long step;
};
struct MyDataBlock{
    unsigned int size;
    char *data;
};


struct BoardObject{
    GameCore *core;
    Point pos;
    Point _next_pos;
    float z=0;
    float angle=0;
    float angle_z=0;
    float r;
    mutable ViewShadow *view=nullptr;
    OBJ_ID id;
    bool for_remove=false;
    union{
        STEP_VALUE last_update_from_server_time=0;//IN SERVER side this parametr means
        STEP_VALUE last_event_time;
    };
    STEP_VALUE my_step=0;
    //BoardObject(const Point &p):pos(p){}
    BoardObject(GameCore *core);
    void remove();

    virtual void get_data(BoardObjectState *s)const;
    virtual MyDataBlock get_data()const;
    virtual void set_data(const BoardObjectState *state);

    PostionConfig getPosConfig(){
        return PostionConfig{pos=pos,angle=angle};
    }
};

PAK_STRUCT LiveBoardObjectState :public BoardObjectState{
    float speed;
    int max_healt;
    int healt;
};

struct LiveBoardObject:public BoardObject{
    float speed=0;
    Point v;
    float v_z;
    int max_healt;
    int healt;

    map<long,PostionConfig> history;

    LiveBoardObject(GameCore *core):BoardObject (core){}
    void remove();
    virtual void update();


    virtual void get_data(BoardObjectState *s)const;
    virtual MyDataBlock get_data()const;
    virtual void set_data(const BoardObjectState *state);
};

struct BoardObjectSyncData{
    GameCore *game_core=nullptr;
    OBJ_ID id;
    virtual void get_data(BoardObjectState *s)const=0;
    virtual void run_game_core(const BoardObjectState *s)=0;

};


#endif // BOARDOBJECT_H
