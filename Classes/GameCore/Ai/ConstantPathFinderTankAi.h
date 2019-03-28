#ifndef CONSTANTPATHFINDERTANKAI_H
#define CONSTANTPATHFINDERTANKAI_H
#include "GameCore/Tank.h"
#include "tools/MapGraph.h"
#include <vector>
using namespace std;
//using namespace MapGraph;

class ConstantPathFinderTankAi:public TankAi
{
public:
    MapGraph::MapGraph *graph;
    vector<Point> array;


    void refresh();

    size_t currentTargetIndex=0;
    ConstantPathFinderTankAi(Tank *target);
    virtual void step()override;
};

#endif // CONSTANTPATHFINDERTANKAI_H
