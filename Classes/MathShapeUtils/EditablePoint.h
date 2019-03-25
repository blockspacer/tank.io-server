#ifndef EDITABLEPOINT_H
#define EDITABLEPOINT_H
#include "cocos2d_server/cocos2d_server.h"
#ifdef EDITOR_MODE

struct EditablePoint
{
    virtual void change(const Point &p)=0;
    virtual Point getPos()=0;
};
#else
struct EditablePoint
{
};
#endif

#endif // EDITABLEPOINT_H
