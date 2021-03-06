#ifndef GAMECORE_H
#define GAMECORE_H


#include "vector"
#include "list"
#include <set>
#include "BoardObject.h"

using namespace std;

struct Misle;
struct Tank;
struct CircleBlock;
struct PlatformLine;
struct ViewHandler{
    //virtual void handle_board(BoardObject *bo)=0;
    GameCore *game_core=nullptr;
    virtual void handle_shot(Misle *shot)=0;
    virtual void handle_tank(Tank *tank)=0;
    virtual void handle_block(CircleBlock *block)=0;
    virtual void handle_block(PlatformLine *block)=0;
    virtual void triger_change(int object_id)=0;




};





struct GameCore;


struct GameConf{
    int shot_count=0;
    float total_time=0;
    struct TargetConf{
        float angle;
        float speed;
    }target;
    int time_events_iterator;
    int pin_events_iterator;
};




struct Tank;
struct Unit;


struct Player{
    USER_ID user_id=0;
    Tank *tank=nullptr;
};

class GameCore
{


    map<int,Misle* > shots;


public:
    map<OBJ_ID,BoardObject *> zombies;

    map<OBJ_ID,BoardObject *> all_objects;
    ~GameCore(){
        CCLOG("~GameCore");
    }
    float max_d;
    bool server=false;
    bool terminal=false; //wich GamecOre instance maybe be both the server and terminal at same time
    map<int,Misle* > &get_shots(){
        return shots;
    }
    map<int,Tank* > tanks;
    set<PlatformLine*> lines;
    set<CircleBlock*> blocks; //TODO for beter performance change to kind of vector
    map<USER_ID,Player*> players;
    ViewHandler *view_handler=nullptr;
    static double PI;

    Player *my_player=nullptr;




    vector<GameConf> confs;



    vector<BoardObject*> targets;
    bool is_live=true;
    bool is_win;
    TIME_VALUE start_time=0;
    STEP_VALUE total_time=0;
    TIME_VALUE milisecond_per_step=20;
    float step_time;

    long long get_step_with_time(TIME_VALUE time){
        return (time-start_time)/milisecond_per_step;
    }
    TIME_VALUE get_milisecond_time(TIME_VALUE time){
        return (time-start_time);
    }


    GameCore();
    void rand_prize();

    static float angle_refresh_pi_npi(float x){
        if(x<0)
          x+=int(-x/PI/2+2)*2*PI;
        x-=int(x/PI/2)*2*PI;
        if(x>PI)
            return x-2*PI;
        return x;
    }

    void load_sample();
    void load(int lvl);
    bool check_collision();
    void update(bool fast=true);
    void init();
    Player *add_player_with_tank(long long);
    void set_tank(long long player_id,
                  float room_time,
                  Point pos,
                    float speed,
                    float angle
                            );

    int __shot_id=0;
    int generate_shot_id(){
        return ++__shot_id;
    }
    //void set_shot(int shot_id,int tank_id, float time, Point start, Point velocity);

    CircleBlock *create_block();
    PlatformLine *create_platform_line(Point s,Point f);
    void init_block(CircleBlock *);

    void init_platform_line(PlatformLine*);

    Misle* create_shot();
    void init_shot(Misle *);


    Tank* create_tank();
    void init_tank(Tank *);
    GameConf get_conf();
    Tank* get_tank(int id);
    Player* get_player(USER_ID id);
    BoardObject *get_object(OBJ_ID id);

    void add_object(BoardObject *obj){
        zombies[obj->id]=obj;
        all_objects[obj->id]=obj;
        if(server)
            obj->last_event_time=total_time;
    }

    BoardObject *get_live_or_zombiee(OBJ_ID id){
        if(zombies.find(id)==zombies.end())
            return nullptr;
        return zombies[id];
    }


};

#endif // GAMECORE_H
