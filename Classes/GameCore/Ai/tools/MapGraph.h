#ifndef MAPGRAPH_H
#define MAPGRAPH_H
#include "GameCore/PlatformLine.h"
#include "MathShapeUtils/SegmentLine.h"
#include "MathShapeUtils/EditablePoint.h"
#include <vector>

namespace MapGraph{
    struct Edge{
        int v;
        SegmentLine l;
        float minBlockDistance;
    };
    struct Node:public EditablePoint{
        int id;
        Point pos;
        vector<Edge> neighbors;
        #ifdef EDITOR_MODE
        void change(const Point &p){
            pos=p;
        }
        #endif
    };


    struct MapGraph
    {
        GameCore *core;
        vector<Node> nodes;
        MapGraph(GameCore *core);


        int generateNodeId(){
            return nodes.size();
        }
        int pushNode(Point p){
            Node n;
            n.pos=p;
            nodes.push_back(n);
            return n.id=nodes.size()-1;
        }
        // yek jadaval xn dar yn misaze
        void init3_3(Point center,float w,float h,int xn,int yn);

        // gharer noghati ro dore block ha bezare ke masir optimum ro  dar biyare ghotre tank va deghati ke dayre ro takhmin bayad bezane lazeme
        void add_nodes(float tank_r,float d);


        bool checkLine(Point s,Point f,float r);
        void refreshEdge(int v,int e_idx);

        void createEdges(float distance,float tank_r);
    };
}
#endif // MAPGRAPH_H
