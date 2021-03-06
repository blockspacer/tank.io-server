#ifndef MISLE_H
#define MISLE_H
#include "BoardObject.h"
PAK_STRUCT MisleState:public LiveBoardObjectState{
                                 OBJ_ID owner_id;
                                 OBJ_ID target_tank_id;
                                 float max_dis;
                                 float angle_speed=1;
                                 int damage;

};
struct Misle:public LiveBoardObject{
    OBJ_ID owner_id;
    OBJ_ID target_tank_id;

    float max_dis;
    float angle_speed=1;
    int damage;
    Misle(GameCore *core):LiveBoardObject(core){
        r=10;
    }

    void update();
    void miss();
    void clear_collision();
    void get_data(BoardObjectState *s)const override;
    virtual MyDataBlock get_data()const override;
    virtual void set_data(const BoardObjectState *state);

};
#endif // MISLE_H
