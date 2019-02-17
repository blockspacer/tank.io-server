#ifndef PLATFORMLINE_H
#define PLATFORMLINE_H
#include "BoardObject.h"

PAK_STRUCT PlatformLineState :public BoardObjectState{
    ZIP_Point start,finish;
};

struct PlatformLine:public  BoardObject
{
    Point mozdavaj;
    Point direction;
    Point amud;
    float h_d;
public:
    Point start,finish;
    //PlatformLine(GameCore *core);


    PlatformLine(GameCore *,Point pos,Point _p,float h_d);
    PlatformLine(GameCore *,Point s,Point f);

    void refresh();

    void get_data(BoardObjectState *s)const;
    virtual void set_data(const BoardObjectState *state);

};

#endif // PLATFORMLINE_H
