#ifndef ABSTRACTTANKVIEWHANDLER_H
#define ABSTRACTTANKVIEWHANDLER_H
#include "cocos2d_server/cocos2d_server.h"
struct BoardObject;
struct Unit;
struct Misle;
struct ViewShadow{
    const BoardObject *obj;
    ViewShadow(const BoardObject *obj):obj(obj){}

    virtual void on_ded()=0;

};
struct AbstractTankViewHandler:public ViewShadow{
    const Unit *unit;
    AbstractTankViewHandler(const Unit *unit):
        ViewShadow((const BoardObject*)unit),unit(unit){}
    virtual void on_ded(){};//=0;
    virtual void show_shot_animation(float shot_dt_time)=0;
    virtual void show_cross_with_block(Point pos,float shedat)=0;
};

struct TankShotViewHandler:public ViewShadow{
    const Misle *misle;
    TankShotViewHandler(const Misle *misle):
        ViewShadow((const BoardObject*)misle),misle(misle){}
    virtual void on_ded(){}
};


#endif // ABSTRACTTANKVIEWHANDLER_H
