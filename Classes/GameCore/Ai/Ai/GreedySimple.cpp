#include "GreedySimple.h"

GreedySimple::GreedySimple(Tank *target):TankAi(target)
{

}
void GreedySimple::step(){
    for(auto  t: target->core->tanks){
        bool enmy=t.second->team_index==0 || t.second->team_index!=target->team_index;

    }
}
