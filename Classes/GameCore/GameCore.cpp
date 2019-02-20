#include "GameCore.h"
#include "Tank.h"
#include "PlatformLine.h"
#include "Misle.h"


GameCore::GameCore()
{
 CCLOG("GameCore::GameCore");

}
void GameCore::add_player_with_tank(long long user_id){
    Player *p=players[user_id];
    if(p==nullptr){
        players[user_id]=p=new Player();
        p->user_id=user_id;
    }
    CCLOG("add_player_with_tank call");
    if(p->tank!=nullptr)
        return ;

    Tank *tank=create_tank();

    tank->user_id=user_id;
    tank->r=100;
    tank->pos={100,100};
    tank->angle=PI/2;
    tank->speed=300;
    p->tank=tank;
    init_tank(tank);
}
void GameCore::set_tank(long long player_id, float room_time, Point pos,float speed, float angle){
    CCLOG("set_tank");
    if(tanks.find(player_id)==tanks.end())
        add_player_with_tank(player_id);
    CCLOG("set_tank 2");
    Tank *tank=tanks[player_id];
    tank->pos=pos;
    tank->speed=speed;
    tank->angle=angle;


}

void GameCore::init(){
    max_d=5000;
    step_time=milisecond_per_step*0.001;
    if(server ){
        if(false)for(int i=0; i<1; ++i)
            for(int j=0; j<1; ++j){
                CircleBlock *c=create_block();
                c->r=300+rand()%300;
                c->pos=Point(i*1200+rand()%600,j*1200+rand()%600);
                init_block(c);
            }
        //PlatformLine *p=create_platform_line({0,0},{1000,1000});
        //init_platform_line(p);
        {
            PlatformLine *p=create_platform_line({500,500},{1000,1000});
            init_platform_line(p);
            CircleBlock *c=create_block();
            c->r=0;
            c->pos=Point(500,500);
            init_block(c);
        }
        {
            PlatformLine *p=create_platform_line({1000,1000},{0,2000});
            init_platform_line(p);

            CircleBlock *c=create_block();
            c->r=0;
            c->pos=Point(0,2000);
            init_block(c);
        }
        if(true){
                        CircleBlock *c=create_block();
                        c->r=200;
                        c->pos=Point(1000,1000);
                        init_block(c);
         }

    }


}
CircleBlock *GameCore::create_block(){
    auto t=new CircleBlock(this);
    t->id=generate_shot_id();
    return t;
}
PlatformLine *GameCore::create_platform_line(Point s,Point f){
    auto t=new PlatformLine(this,s,f);
    t->id=generate_shot_id();
    return t;
}

void GameCore::init_block(CircleBlock *cblock){
    blocks.insert(cblock);
    all_objects[cblock->id]=cblock;
    if(cblock->view==nullptr)
        view_handler->handle_block(cblock);
}
void GameCore::init_platform_line(PlatformLine *lblock){
    lines.insert(lblock);
    all_objects[lblock->id]=lblock;
    if(lblock->view==nullptr)
        view_handler->handle_block(lblock);
}

Misle *GameCore::create_shot(){

    Misle *t=new Misle(this);
    t->id=generate_shot_id();
    return t;

}

void GameCore::init_shot(Misle *misle){
    CCLOG("init_shot %lld \n",misle->id);
    shots[misle->id]=misle;
    all_objects[misle->id]=misle;
     if(misle->view==nullptr)
        view_handler->handle_shot(misle);
}



Tank *GameCore::create_tank(){
    CCLOG("create tank");
    Tank *t=new Tank(this);
    t->id=generate_shot_id();
    return t;
}
Tank *GameCore::get_tank(int id){
    if(tanks.find(id)==tanks.end())
        return nullptr;
    return tanks[id];

}
Player *GameCore::get_player(USER_ID id){
    if(players.find(id)==players.end())
        return nullptr;
    return players[id];

}

BoardObject *GameCore::get_object(int id){
    if(all_objects.find(id)==all_objects.end())
        return nullptr;
    return all_objects[id];

}

void GameCore::init_tank(Tank *misle){
    tanks[misle->id]=misle;
    all_objects[misle->id]=misle;

    if(misle->view==nullptr)
        view_handler->handle_tank(misle);
}



double GameCore::PI=3.14159265358979323846 ;

void GameCore::update(bool fast){
    ++total_time;
    float dt=step_time;

    vector<int> for_remove;
    for(auto t:shots)if(t.second==nullptr || t.second->for_remove){
        for_remove.push_back(t.first);
    }
    for(auto t:for_remove){
        tanks.erase(t);
        shots.erase(t);
        all_objects.erase(t);
    }

    for(auto t:shots)if(t.second)
        t.second->update();
    for(auto t:tanks)if(t.second)
        t.second->update();

    check_collision();


    for(auto t:shots)if(t.second)
        t.second->pos=t.second->_next_pos;
    for(auto t:tanks)if(t.second)
        t.second->pos=t.second->_next_pos;




}
bool GameCore::check_collision(){
    for(auto t:tanks)if(t.second)
        t.second->clear_collision();

    for(auto t:shots)if(t.second)
        t.second->clear_collision();
    return false;
}

GameConf GameCore::get_conf(){
    GameConf c;
    return c;
}

void GameCore::load_sample(){

}
void GameCore::load(int lvl){




}
