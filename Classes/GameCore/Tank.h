#ifndef TANK_H
#define TANK_H
#include "BoardObject.h"
#include "list"
#include <memory>
#include <vector>
struct Tank;
struct Unit;
struct AbstractTankViewHandler;
struct Misle;





struct CirclePartShape{
    Point global_pos;
    Point _pos;
    float r;
    Point sum_of_force;
};
PAK_STRUCT UnitState:public LiveBoardObjectState{

    USER_ID user_id;
    float target_angle;
    float max_angle_speed;
    float angle_speed;
};
struct Unit:public LiveBoardObject{

    USER_ID user_id;
    float target_angle=0;
    float clint_side_target_angle=0;
    float jo_angle;
    AbstractTankViewHandler *view_handler=nullptr;
    float max_angle_speed=0.3*3.1415;
    float angle_speed;

    //float target_angle=0;

    vector<CirclePartShape> parts;
    Unit(GameCore *core):LiveBoardObject(core){
        healt=max_healt=100;
    }
    virtual void update()=0;

    float get_health_perecentage()const{
        return healt*100/max_healt;
    }
    void get_damage(int x){
        healt-=x;
    }

    void clear_collision();



    virtual void get_data(BoardObjectState *s)const;
    virtual void set_data(const BoardObjectState *state);
};


struct Tower:public LiveBoardObject{

};


PAK_STRUCT TankState:public UnitState{


};
struct Tank:public Unit{




    void update_inputs();
    void update_angle();
    void update();

    void set_target_angle(float t_angle);

    Misle *shot(OBJ_ID shot_id);
    Tank(GameCore *core);


    virtual void get_data(BoardObjectState *s)const;
    virtual void set_data(const BoardObjectState *state);

    struct Input{
        int id;
        int client_id;
        STEP_VALUE trigger_step;
        PAK_STRUCT State{
            unsigned int size_of_data;
            enum Type{
                MOVMENT=1,
                SHOT=2,
            }type;
            OBJ_ID tank_id;
            STEP_VALUE trigger_step;
            unsigned int id;
            unsigned int client_id;
            bool check(){
                return true;
            }
        };
        virtual void run_on_this(Tank *thiz)const=0;
        bool operator<(const Input &i)const{
            if(trigger_step!=i.trigger_step)
                return trigger_step<i.trigger_step;
            return id<i.id;
        }
        struct ptr:public std::shared_ptr<Input>{
            bool operator<(const ptr &i)const{
                return *get()<*i.get();
            }
            ptr(Input *p):std::shared_ptr<Input>(p){}
            ptr():std::shared_ptr<Input>(nullptr){}
        };
        virtual void get_data(Input::State *data)const ;
        virtual void set_data(const Input::State *data);
    };
    struct InputFarman: public Input{
          float target_farman;
          PAK_STRUCT State:public Input::State{
              float farman;
              bool check(){
                  return true;
              }
          };
          virtual void run_on_this(Tank *thiz)const{
              thiz->target_angle=target_farman;
          }
          virtual void get_data(Input::State *data)const;
          virtual void set_data(const Input::State *data);
    };
    struct InputShot: public Input{
        OBJ_ID shot_id;
        PAK_STRUCT State:public Input::State{
            int shot;
            OBJ_ID shot_id;
            bool check(){
                return true;
            }
        };
        virtual void run_on_this(Tank *thiz)const;
        virtual void get_data(Input::State *data)const;
        virtual void set_data(const Input::State *data);
    };

    std::multimap<STEP_VALUE,Input::ptr > inps;
    std::map<int,Input::ptr> cinps;
    std::map<int,Input::ptr> sinps;
    virtual InputFarman* add_action(STEP_VALUE action_time,float f,int client_id);
    virtual InputShot* add_shot_action(STEP_VALUE action_time,int client_id);
    virtual Input* get_action(int client_id);
    virtual void show_shot_animation(STEP_VALUE shot_time);
    void add_created_action(Input *inp);
};



PAK_STRUCT CircleBlockState:BoardObjectState{

};

struct CircleBlock:public BoardObject{
    CircleBlock(GameCore *core):BoardObject(core){}
    void get_data(BoardObjectState *s)const override;
    virtual void set_data(const BoardObjectState *state)override;
};


#endif // TANK_H
