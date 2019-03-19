#include "RandomTankAi.h"
#include "GameCore/Tank.h"
#include "MathShapeUtils/Base.h"
#include "GameCore/GameCore.h"
RandomTankAi::RandomTankAi(Tank *target):TankAi(target)
{

}
void RandomTankAi::step(){

}



ConstantTankAi::ConstantTankAi(Tank *target):TankAi(target)
{

}
void ConstantTankAi::step(){
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
    }
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
    }
}
