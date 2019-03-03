#ifndef BOARDOBJECTEVENT_H
#define BOARDOBJECTEVENT_H
#include "BoardObject.h"

struct BoardObjectEvent : public BoardObjectSyncData{
    OBJ_ID target_object_id;
    STEP_VALUE core_time;
    virtual void get_data(BoardObjectState *S)const;
    virtual MyDataBlock get_data()const;
    virtual void run_game_core(const BoardObjectState *s);
};

#endif // BOARDOBJECTEVENT_H
