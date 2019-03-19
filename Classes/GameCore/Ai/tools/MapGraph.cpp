#include "MapGraph.h"
#include "GameCore/GameCore.h"
#include "GameCore/PlatformLine.h"
#include "GameCore/Tank.h"
namespace MapGraph{
    MapGraph::MapGraph(GameCore *core):core(core)
    {

    }

    void MapGraph::init3_3(Point center, float w, float h, int xn, int yn){
        for(int i=0; i<xn; ++i)
            for(int j=0; j<yn; ++j){
                pushNode(center+Point(w*i/xn,h*j/yn));
            }

    }

    void MapGraph::add_nodes(float tank_r, float d){
        for(auto t : core->blocks){
            float r=t->r+tank_r;
            int pn=12;
            r*=1.0/cos(3.1415*2/pn);
            for(int i=0; i<pn;++i)
                pushNode(t->pos+Point(cos(i*3.1415*2/pn),sin(i*3.1415*2/pn)));
        }
        for(auto t : core->lines){
            float r=tank_r;
            int pn=12;
            r*=1.0/cos(3.1415*2/pn);
            for(int i=0; i<pn;++i){
                pushNode(t->pos+t->l.start+Point(cos(i*3.1415*2/pn),sin(i*3.1415*2/pn)));
                pushNode(t->pos+t->l.finish+Point(cos(i*3.1415*2/pn),sin(i*3.1415*2/pn)));
            }
        }

    }

    bool MapGraph::checkLine(Point s, Point f, float r){
        SegmentLine l(s,f);
        for(auto t:core->blocks){
            float d=l.get_distance(t->pos);
            if(d<r)
                return false;
        }
        for(auto t:core->lines)
            if(t->l.have_intersec(s,f,r))
                return false;
        return true;


    }

    void MapGraph::refreshEdge(int v, int e_idx){
        Edge &e=nodes[v].neighbors[e_idx];
        e.l.refresh();
        for(auto t:core->blocks){
            float d=e.l.get_distance(t->pos);
            d-=t->r;
            if(e.minBlockDistance>d)
                e.minBlockDistance>d;
        }
        /*for(auto t:core->lines){
            float d=e.l.get_distance(t->l);
            if(e.minBlockDistance>d)
                e.minBlockDistance>d;
        }TODO /**/


    }

    void MapGraph::createEdges(float distance,float tank_r){
        for(size_t i=0; i<nodes.size(); ++i)
            for(size_t j=0; j<nodes.size(); ++j)if(i!=j){
                Point d=nodes[i].pos-nodes[j].pos;
                if(d.length()<distance){

                    if(!checkLine(nodes[i].pos,nodes[j].pos,tank_r))
                         continue;
                    SegmentLine s(nodes[i].pos,nodes[j].pos);
                    Edge e;
                    e.v=j;
                    //e.minBlockDistance
                    e.l=s;
                    nodes[i].neighbors.push_back(e);
                }
            }
    }
}
