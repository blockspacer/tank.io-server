#include "ConstantPathFinderTankAi.h"



#include "GameCore/Tank.h"
#include "MathShapeUtils/Base.h"
#include "GameCore/GameCore.h"

void ConstantPathFinderTankAi::refresh(){
    vector<Point> path2;
    Point prv=array[0];
    for(size_t i=1; i< array.size()+1;++i){
        Point s=prv;
        Point f=array[i % array.size()];
        path2.push_back(s);
        auto t=graph->findPath(s,f,500,100);
        if(t.d>10000){
            i+=1;
            continue;
        }
        prv=f;
        for(auto tmp:t)
            path2.push_back(tmp);
    }
    array=path2;
#ifdef EDITOR_MODE
    graph->view_handler->show_path(path2);
#endif
}

ConstantPathFinderTankAi::ConstantPathFinderTankAi(Tank *target):TankAi(target)
{

}
void ConstantPathFinderTankAi::step(){
    Point p=array[currentTargetIndex];
    p-=target->pos;
    float angle=Base2D::find_angle<Point>(p);
    //target->target_angle=angle;

    //target->speed=10000;
    target->add_action(target->core->total_time,angle,1000,0);
    if(p.lengthSquared()<200){
        currentTargetIndex++;
        if(currentTargetIndex>=array.size())
            currentTargetIndex=0;
        return ;
    }


}

