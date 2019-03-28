#ifndef GREEDYSIMPLE_H
#define GREEDYSIMPLE_H
#include "GameCore/Tank.h"
#include "../tools/MapGraph.h"
#include <vector>
using namespace std;

class GreedySimple:public TankAi
{
public:

    GreedySimple(Tank *target);
    virtual void step()override;
};


#endif // GREEDYSIMPLE_H
