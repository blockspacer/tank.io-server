#ifndef MAPGRAPH_H
#define MAPGRAPH_H
#include "GameCore/PlatformLine.h"
#include "GameCore/GameCore.h"
#include "MathShapeUtils/SegmentLine.h"
#include "MathShapeUtils/EditablePoint.h"
#include <vector>

namespace MapGraph{


    typedef size_t NodeId;
    struct Edge{
        NodeId v;
        SegmentLine l;
        float minBlockDistance;
    };
    struct Node{
        NodeId id;
        Point pos;
        vector<Edge> neighbors;
        #ifdef EDITOR_MODE
        struct VectorPointer:public EditablePoint{
            vector<Node> *v;
            size_t idx;
            VectorPointer(vector<Node> *v,size_t idx):v(v),idx(idx){}
            Point getPos(){
                return (*v)[idx].pos;
            }
            void change(const Point &p){
                (*v)[idx].pos=p;
            }

        };

        #endif
    };
    struct Path:public vector<Point>{
        float d;
    };
    #ifdef EDITOR_MODE
    struct AiToolsEditorAbstract{
        virtual void handle_Node(Node::VectorPointer *)=0;
        virtual void show_path(const vector<Point> &v)=0;
    };
    #else
    struct AiToolsEditorAbstract{
    };
    #endif


    struct MapGraph
    {
        GameCore *core=nullptr;
        AiToolsEditorAbstract *view_handler=nullptr;
        vector<Node> nodes;

        struct shortPath{
           NodeId nextV;
           float pathDistance;
        };
        shortPath *belman;
        MapGraph(GameCore *core);


        NodeId generateNodeId(){
            return nodes.size();
        }
        NodeId pushNode(Point p){
            Node n;
            n.pos=p;
            n.id=nodes.size();
            nodes.push_back(n);
            #ifdef EDITOR_MODE
            if(core!=nullptr && core->view_handler!=nullptr){
                auto t=new Node::VectorPointer(&nodes,nodes.size()-1);
                view_handler->handle_Node(t);
            }
            #endif


            return n.id;
        }
        // yek jadaval xn dar yn misaze
        void init3_3(Point center,float w,float h,int xn,int yn);

        // gharer noghati ro dore block ha bezare ke masir optimum ro  dar biyare ghotre tank va deghati ke dayre ro takhmin bayad bezane lazeme
        void add_nodes(float tank_r,float d);


        bool checkLine(Point s,Point f,float r);
        void refreshEdge(NodeId v,int e_idx);

        void createEdges(float distance,float tank_r);



        void belMan();




        vector<NodeId> findNodes(Point center,float distance,float tank_r);

        Path findPath(Point s,Point f,float distance,float tank_r);


    };
}
#endif // MAPGRAPH_H
