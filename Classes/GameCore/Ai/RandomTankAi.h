#ifndef RANDOMTANKAI_H
#define RANDOMTANKAI_H
#include "GameCore/Tank.h"
#include <vector>
using namespace std;
class RandomTankAi:public TankAi
{
public:
    RandomTankAi(Tank *target);
    void step();
};

class ConstantTankAi:public TankAi
{
public:
    vector<Point> array;
    size_t currentTargetIndex=0;
    ConstantTankAi(Tank *target);
    virtual void step()override;
};



class ConstantPathFinderTankAi:public TankAi
{
public:
    vector<Point> array;
    size_t currentTargetIndex=0;
    ConstantPathFinderTankAi(Tank *target);
    virtual void step()override;
};
#endif // RANDOMTANKAI_H
