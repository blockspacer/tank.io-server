/*
 * rbtree.h
 * An implementation of LLRB
 */
#ifndef MY__RBTREE_H
#define MY__RBTREE_H
#include <iostream>


template<typename Value>
class RBTree{

 public:
  enum Color{
    RED,
    BLACK
  };

  struct Node{
    Color color;
    Node *leftNode=nullptr;
    Node *rightNode=nullptr;
    Node *parent=nullptr;
    int childs_number;
    Value value;
    Node(const Value &vv):value(vv),childs_number(1){

    }
    void set_left(Node *n){

        leftNode=n;
        if(n!=nullptr)
            n->parent=this;
        refresh_child();
    }
    void set_rigth(Node *n){
        rightNode=n;
        if(n!=nullptr)
            n->parent=this;
        refresh_child();
    }
    void refresh_child(){
        int bch=childs_number;
        childs_number=1;
        if(rightNode!=nullptr)
           childs_number+=rightNode->childs_number;
        if(leftNode!=nullptr)
           childs_number+=leftNode->childs_number;
        if(bch!=childs_number && parent!=nullptr)
            parent->refresh_child();
    }


    Node *get_min(){
        Node *r=this;
        while(r->leftNode)
            r=r->leftNode;
        return r;
    }
    static Color check_color(const Node *n){
        if(n==nullptr)
            return BLACK;
        return n->color;

    }
    static void set_color(Node *n,Color c){
        if(n==nullptr)
            return ;
        n->color=c;

    }


  };
  struct my_iterator{
      Node *n;
      my_iterator operator++(){

          Value v=n->value;
        if(n->rightNode!=nullptr){
            n=n->rightNode->get_min();
            if(n->value<v){
                std::cerr<<"ya khoda"<<std::endl;
            }
        }else{

            do{
                Node *p=n->parent;
                if(p==nullptr){
                    n=nullptr;
                    return *this;
                }
                if(p->leftNode==n){
                    n=p;
                    break;
                }
                n=p;
            }while(true);
            //if(n->rightNode!=nullptr)
             //   n=n->rightNode->get_min();
            if(n->value<v){
                std::cerr<<"ya khoda"<<std::endl;
            }
        }
        return *this;
      }
  };

  
 public:
  RBTree();
  ~RBTree();
  void insert(const Value & value);
  my_iterator search(const Value & value,int &out);
  size_t size();
  const  my_iterator by_index(int index,Node *n=nullptr);

  //debug
  void run_test();
  my_iterator begin();
  my_iterator end();
  void replace_node(Node *dst,Node *src);
  bool  is_leaf(Node *n);

  Node * sibling(Node *n,Node *n_parent);
  void  delete_case1(Node *n,Node *n_parent);
  void  delete_case2(Node *n,Node *n_parent);
  void  delete_case3(Node *n,Node *n_parent);

  void  delete_case4(Node *n,Node *n_parent);
  void  delete_case5(Node *n,Node *n_parent);
  void  delete_case6(Node *n,Node *n_parent);

  void  delete_one_child(Node *n);


  Node * TREE_SUCCESSOR(const Node *z);
  Node * TREE_MINOR(const Node *z);

  const Node * RB_DELETE(Node *z);
  void remove_leaf(const Node *z);
  void RB_DELETE_FIXUP(Node *x);


  void  my_delete(Node *z);
  void  my_delete(const Value &z);
  Node *_root=nullptr;
 private:

  Node* initNode(const Value &value);

  //insert
  Node* recursive_insertion(Node *parent, const Value &value);
  Node* right_rotation(Node *node);
  Node* left_rotation(Node *node);
  void change_left_child(Node *node,Node *child);
  void change_rigth_child(Node *node,Node *child);

  bool is_red(Node *node);
  void flip_colors(Node *node);

  //search
  Node* recursive_search(Node *node, const Value &value,int &out);


  //delete all memory allocated
  void deleteAll(Node *root);

  //debug
  void print_node(Node *node, int height);
  int test_rb_properties(Node *node);
};
#include "rbtree.inl"
#endif
