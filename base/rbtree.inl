#include "rbtree.h"



template<typename Value>
RBTree<Value>::RBTree(){
}
template<typename Value>
RBTree<Value>::~RBTree(){
  deleteAll(_root);
}
template<typename Value>
void RBTree<Value>::deleteAll(Node *node){
  if(node != NULL){
    Node *leftNode = node->leftNode;
    Node *rightNode = node->rightNode;

    deleteAll(node->leftNode);
    deleteAll(node->rightNode);
    
    delete node;
  }

}


template<typename Value>
void RBTree<Value>::replace_node(RBTree<Value>::Node *n,RBTree<Value>::Node *v)
{
    Node *n_parent=n->parent;
    if(n_parent==nullptr)
        _root=v;
    else{
        if(n_parent->leftNode==n)
            n_parent->set_left(v);
        if(n_parent->rightNode==n)
            n_parent->set_rigth(v);
    }
    if(v!=nullptr){
        v->set_left(n->leftNode);
        v->set_rigth(n->rightNode);
    }

}
template<typename Value>
bool RBTree<Value>::is_leaf(RBTree<Value>::Node *n)
{
    return n==nullptr;
    if(n==nullptr)
        return true;
}

template<typename Value>
typename RBTree<Value>::Node * RBTree<Value>::sibling(RBTree<Value>::Node *n,RBTree<Value>::Node *n_parent)
{
    if(n_parent==nullptr)
        return nullptr;
    if(n_parent->leftNode==n)
        return n_parent->rightNode;
    return n_parent->leftNode;
}



template<typename Value>
void RBTree<Value>::delete_case1(RBTree<Value>::Node *n,RBTree<Value>::Node *n_parent)
{
    if (n_parent != nullptr)
      delete_case2(n,n_parent);
}
template<typename Value>
void RBTree<Value>::delete_case2(RBTree<Value>::Node *n,RBTree<Value>::Node *n_parent)
{
    RBTree<Value>::Node *s = sibling(n,n_parent);

    if (Node::check_color(s) == RED) {// null is BLACK
     n->parent->color = RED;
     s->color = BLACK;
     if (n == n_parent->leftNode)
      left_rotation(n_parent);
     else
      right_rotation(n_parent);
    }
    delete_case3(n,n_parent);
}
template<typename Value>
void RBTree<Value>::delete_case3(RBTree<Value>::Node *n,RBTree<Value>::Node *n_parent)
{
 RBTree<Value>::Node *s = sibling(n,n_parent);

 if ((Node::check_color(n_parent) == BLACK) &&
     (Node::check_color(s) == BLACK) &&
     (Node::check_color(s->leftNode) == BLACK) &&
     (Node::check_color(s->rightNode) == BLACK)) {
  s->color = RED;
  delete_case1(n_parent,n_parent->parent);
 } else
  delete_case4(n,n_parent);
}

template<typename Value>
void RBTree<Value>::delete_case4(RBTree<Value>::Node *n,RBTree<Value>::Node *n_parent)
{
    RBTree<Value>::Node *s = sibling(n,n_parent);

     if ((n_parent->color == RED) &&
         (Node::check_color(s)== BLACK) &&
         (Node::check_color(s->leftNode) == BLACK) &&
         (Node::check_color(s->rightNode) == BLACK)) {
      s->color = RED;
      n_parent->color = BLACK;
     } else
      delete_case5(n,n_parent);
}
template<typename Value>
void RBTree<Value>::delete_case5(RBTree<Value>::Node *n,RBTree<Value>::Node *n_parent)
{
    RBTree<Value>::Node *s = sibling(n,n_parent );

     if  (Node::check_color(s) == BLACK) { /* this if statement is trivial,
    due to case 2 (even though case 2 changed the sibling to a sibling's child,
    the sibling's child can't be RED, since no RED parent can have a RED child). */
    /* the following statements just force the RED to be on the left of the left of the parent,
       or right of the right, so case six will rotate correctly. */
      if ((n == n->parent->leftNode) &&
          (s->rightNode->color == BLACK) &&
          (s->leftNode->color == RED)) { /* this last test is trivial too due to cases 2-4. */
       s->color = RED;
       s->leftNode->color = BLACK;
       right_rotation(s);
      } else if ((n == n_parent->rightNode) &&
                 (Node::check_color(s->leftNode) == BLACK) &&
                 (Node::check_color(s->rightNode) == RED)) {/* this last test is trivial too due to cases 2-4. */
       s->color = RED;
       s->rightNode->color = BLACK;
       left_rotation(s);
      }
     }
     delete_case6(n,n_parent);
}

template<typename Value>
void RBTree<Value>::delete_case6(RBTree<Value>::Node *n,RBTree<Value>::Node *n_parent)
{
    RBTree<Value>::Node *s = sibling(n,n_parent);

     s->color = n_parent->color;
     n->parent->color = BLACK;

     if (n == n->parent->leftNode) {
      s->rightNode->color = BLACK;
      left_rotation(n->parent);
     } else {
      s->leftNode->color = BLACK;
      right_rotation(n->parent);
     }
}

template<typename Value>
void RBTree<Value>::delete_one_child(Node *n)
{
 /*
  * Precondition: n has at most one non-leaf child.
  */
    if(n==nullptr)
        return ;
 RBTree<Value>::Node *child = is_leaf(n->rightNode) ? n->leftNode : n->rightNode;
    if(child==nullptr){
        replace_node(n, nullptr);
        return;
    }
    if(n->leftNode!=nullptr && n->rightNode!=nullptr){
        Node *min=n->rightNode->get_min();
        replace_node(n,child);
        return;
    }
 replace_node(n, child);
 if (n->color == BLACK) {
  if (Node::check_color(child) == RED){ //null is BLACK
   child->color = BLACK;
  }else
     delete_case1(child,n);
 }
 //free(n);
}
template<typename Value>
typename RBTree<Value>::Node* RBTree<Value>::initNode(const Value &value){
  
  RBTree<Value>::Node *node = new RBTree<Value>::Node(value);
  node->childs_number=1;
  node->leftNode = nullptr;
  node->rightNode = nullptr;
  node->color = RED; // when you insert new node, insert the node with RED

  return node;
}
template<typename Value>
void RBTree<Value>::insert(const Value &value){
  _root = recursive_insertion(_root, value);
  _root->color = BLACK; // always BLACK
}

template<typename Value>
typename RBTree<Value>::Node* RBTree<Value>::recursive_insertion(RBTree<Value>::Node *node, const Value &value){

  if(node == nullptr){
    return initNode(value);
  }

  // right insertion
  if(node->value < value){
    //change_rigth_child(node,recursive_insertion(node->rightNode, value));
      node->set_rigth(recursive_insertion(node->rightNode, value));

  // lef insertion
  }else if( value <node->value){
      node->set_left(recursive_insertion(node->leftNode, value));

    //change_left_child(node,recursive_insertion(node->leftNode, value));
  }
  node->refresh_child();
  //right rotation
  if(is_red(node->rightNode)){ //2 nodes
    node = left_rotation(node);
  }
  // left rotation
  if(is_red(node->leftNode) && is_red(node->leftNode->leftNode)){ //3 nodes
    node = right_rotation(node);
  }
  // flip colors (RED -> BLACK, BLACK -> RED)
  if(is_red(node->leftNode) && is_red(node->rightNode)){ //split 4 nodes
    flip_colors(node);
  }
  
  return node; 
}
template<typename Value>
void RBTree<Value>::change_left_child(typename RBTree<Value>::Node *node, RBTree<Value>::Node *child){

    node->set_left(child);




}
template<typename Value>
void RBTree<Value>::change_rigth_child(Node *node, Node *child){


    node->set_rigth(child);


}
template<typename Value>
typename RBTree<Value>::Node* RBTree<Value>::left_rotation(Node *node){
    //left rotation
    //std::cout << "left rotation: " << node->value << std::endl;
    Node *x = node->rightNode;
    change_rigth_child(node,x->leftNode);
    change_left_child(x, node);
    x->color = node->color;
    node->color = RED;

    return x;
}
template<typename Value>
typename RBTree<Value>::Node* RBTree<Value>::right_rotation( RBTree<Value>::Node *node){
  //right rotation
  //std::cout << "right rotation: " << node->value << std::endl;
  Node *x = node->leftNode;
  change_left_child(node,x->rightNode);
  change_rigth_child(x , node);
  x->color = node->color;
  node->color = RED;
  
  return x;
}
template<typename Value>
void RBTree<Value>::flip_colors(Node *node){
  node->color == RED?node->color = BLACK : node->color = RED;
  node->leftNode->color == RED?node->leftNode->color = BLACK : node->leftNode->color = RED;
  node->rightNode->color == RED?node->rightNode->color = BLACK : node->rightNode->color = RED;
}
template<typename Value>
bool RBTree<Value>::is_red(Node *node){
  return node != NULL && node->color == RED;
}
template<typename Value>
void RBTree<Value>::run_test(){
  print_node(_root, 0);
}
template<typename Value>
typename RBTree<Value>::my_iterator RBTree<Value>::search(const Value &value,int &out){
  RBTree<Value>::Node *node = recursive_search(_root, value,out);
  typename RBTree<Value>::my_iterator r;
  r.n=node;
  return r;
}

template<typename Value>
size_t RBTree<Value>::size(){
    if(_root==nullptr)
        return 0;
  return _root->childs_number;
}

template<typename Value>
const typename RBTree<Value>::my_iterator  RBTree<Value>::by_index(int index,Node *n){
    typename RBTree<Value>::my_iterator res;
    if(n==nullptr)
        n=_root;
    if(n==nullptr){
        res.n=nullptr;
        return res;
    }

    int deb=1;
    if(n->rightNode!=nullptr)
        deb+=n->rightNode->childs_number;
    if(n->leftNode!=nullptr)
        deb+=n->leftNode->childs_number;
    if(deb!=n->childs_number)
        std::cerr<<"childs number conflict"<<std::endl;

    if(n->leftNode!=nullptr){
         if(index < n->leftNode->childs_number)
             return by_index(index,n->leftNode);
         else
             index-=n->leftNode->childs_number;


    }

    if(index==0){
        res.n=n;
        return res;
    }
    index-=1;
    if(n->rightNode!=nullptr)
        return by_index(index,n->rightNode);

    res.n=nullptr;
    return res;
}
template<typename Value>
typename RBTree<Value>::Node* RBTree<Value>::recursive_search(typename RBTree<Value>::Node *node, const Value &value,int &out){

  if(node != NULL){
    if( value <node->value){
      node = recursive_search(node->leftNode, value,out);

    }else if(node->value < value){
      int temp=1;
      if(node->leftNode!=nullptr)
         temp+=node->leftNode->childs_number;
      node = recursive_search(node->rightNode, value,out);
      out+=temp;
    }else if(/*node->value == value/**/ true){
        out=0;
        if(node->leftNode!=nullptr)
            out+=node->leftNode->childs_number;
      return node;
    }
  }

  return node;
}


template<typename Value>
typename RBTree<Value>::my_iterator RBTree<Value>::begin(){
  typename RBTree<Value>::my_iterator res;
    res.n=_root->get_min();
  return res;
}

template<typename Value>
typename RBTree<Value>::my_iterator RBTree<Value>::end(){
    typename RBTree<Value>::my_iterator res;
    res.n=nullptr;
  return res;
}

template<typename Value>
void RBTree<Value>::print_node(RBTree<Value>::Node *node, int height){
  if(node != NULL){
    print_node(node->rightNode, height + 1);
    int i = height;
    while(i > 0){
      std::cout << "       ";
      i--;
    }
    //std::cout << (node->color == RED ? "R":"B") << ":" << node->value << "(" << height << ")" << std::endl;
    print_node(node->leftNode, height + 1);
  }
  
}
template<typename Value>
int RBTree<Value>::test_rb_properties(RBTree<Value>::Node *node){
  if(node != NULL && node->color == RED){
    if(node->leftNode->color == RED || node->rightNode->color == RED){
      std::cout << "property violation error (1)" << std::endl;
      test_rb_properties(node->leftNode);
      test_rb_properties(node->rightNode);
      return -1;
    }
  }

  return 0;
}




template <class Value>
typename RBTree<Value>::Node* RBTree<Value>::TREE_SUCCESSOR(const  typename RBTree<Value>::Node *z)
{
    if (z == nullptr || z->rightNode == nullptr)
        return nullptr;
    typename RBTree<Value>::Node *z2=z->rightNode;
    for (; z2->leftNode != nullptr; z2 = z2->leftNode);
    return z2;
}

template <class Value>
typename RBTree<Value>::Node* RBTree<Value>::TREE_MINOR(const  typename RBTree<Value>::Node *z)
{
    if (z == nullptr || z->leftNode == nullptr)
        return nullptr;
    typename RBTree<Value>::Node *z2=z->leftNode;
    for (; z2->rightNode != nullptr; z2 = z2->rightNode);
    return z2;
}

template <class Value>
void RBTree<Value>::remove_leaf(const typename RBTree<Value>::Node *z){
    typename RBTree<Value>::Node*  p=z->parent;
    if(p==nullptr)
        _root=nullptr;
    if(p->leftNode==z)
        p->set_left(nullptr);
    if(p->rightNode==z)
        p->set_rigth(nullptr);
    p->refresh_child();
}



template <class Value>
void RBTree<Value>::my_delete(typename RBTree<Value>::Node *z)
{
    if(z==nullptr)
        return ;
    if (z->leftNode == nullptr && z->rightNode == nullptr){
        remove_leaf(z);
        return;
    }
    if (z->rightNode != nullptr){
        RBTree<Value>::Node *y = TREE_SUCCESSOR(z);
        z->value=y->value;
        my_delete(y);
    }
    if (z->leftNode != nullptr){
        RBTree<Value>::Node *y = TREE_MINOR(z);
        z->value=y->value;
        my_delete(y);
    }

}
template <class Value>
void RBTree<Value>::my_delete(const Value &z){
    int index;
    typename RBTree<Value>::my_iterator i=search(z,index);
    if(i.n!=nullptr)
        my_delete(i.n);
}/**/

template <class Value>
const typename RBTree<Value>::Node* RBTree<Value>::RB_DELETE(typename RBTree<Value>::Node *z)
{
    if(z==nullptr)
        return nullptr;
    RBTree<Value>::Node *x;
    RBTree<Value>::Node *y;
    if (z->leftNode == nullptr || z->rightNode == nullptr)
        y = z;
    else
        y = TREE_SUCCESSOR(z);
    if (y->leftNode != nullptr){
        x = y->leftNode;
    }else{
        x =y->rightNode;
    }
    if(x==nullptr){
        //return nullptr;
        z->value = y->value;
        remove_leaf(y);
        std::cerr<<"null"<<std::endl;
        return nullptr;
    }else{

        z->value = y->value;
        return RB_DELETE(y);;

        std::cerr<<"delete"<<std::endl;
    }
    x->parent = y->parent;
    if (y->parent == nullptr)
        _root = x;
    else if (y == y->parent->leftNode)
            y->parent->set_left(x);
         else
            y->parent->set_rigth(x);
    if (y != z)
    {
        z->value = y->value;
        //z->refresh_child();
        //data__[z] = data__[y];
    }
    if (Node::check_color(y) == BLACK )
        RB_DELETE_FIXUP(x);
    return y;
}

template <class Value>
void RBTree<Value>::RB_DELETE_FIXUP(typename RBTree<Value>::Node *x)
{
    RBTree<Value>::Node *w;
    while (x != _root && Node::check_color(x) == BLACK)
    {
        if (x == x->parent->leftNode)
        {
            w = x->parent->rightNode;
            if ( Node::check_color(w) == RED)
            {
                 Node::set_color(w,BLACK);
                 Node::set_color(x->parent,RED);
                left_rotation(x->parent);
                w = x->parent->rightNode;
            }
            if ( Node::check_color(w->leftNode) == BLACK &&  Node::check_color(w->rightNode) == BLACK)
            {
                 Node::set_color(w,RED);
                x = x->parent;
            }
            else
            {
                if ( Node::check_color(w->rightNode) == BLACK)
                    {
                         Node::check_color(w->rightNode) == BLACK;
                         Node::set_color(w, RED);
                        right_rotation(w);
                        w = x->parent->rightNode;
                    }
                 Node::set_color(w,Node::check_color(x->parent));
                 Node::set_color(x->parent,BLACK);
                 Node::set_color(w->rightNode,BLACK);
                left_rotation(x->parent);
                x = _root;
            }
        }
        else
        {
            w = x->parent->leftNode;
            if ( Node::check_color(w) == RED)
            {
                 Node::set_color(w, BLACK);
                 Node::set_color(x->parent, RED);
                right_rotation(x->parent);
                w = x->parent->leftNode;
            }
            if ( Node::check_color(w->rightNode) == BLACK &&  Node::check_color(w->leftNode) == BLACK)
            {
                 Node::set_color(w, RED);
                x = x->parent;
            }
            else
            {
                if ( Node::check_color(w->leftNode) == BLACK)
                {
                     Node::set_color(w->leftNode, BLACK);
                     Node::set_color(w,RED);
                    left_rotation(w);
                    w = x->parent->leftNode;
                }
                 Node::set_color(w, Node::check_color(x->parent));
                 Node::set_color(x->parent,BLACK);
                 Node::set_color(w->rightNode, BLACK);
                right_rotation(x->parent);
                x = _root;
            }
        }
    }
     Node::set_color(x, BLACK);
}

/*int maindddf(int argc, char **argv){

  //int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  RBTree<int> *rb = new RBTree<int>;

  srand((unsigned)time(NULL));
  int n;
  for(int i = 0; i <1000000; ++i){
    int op=0;
    if(rb->_root!=nullptr && rb->_root->childs_number>100 && rand()%4==1)
        op=1;
    if(op==0){
        n =rand() % 5000000;
        rb->insert(n);
    }else{
        op=rand()%(rb->_root->childs_number);
        //rb->insert(n);
        rb->my_delete(rb->by_index(op).n);
    }

  }
  //rb->run_test();
  std::cout<<std::endl;
  for(int i=0; i<rb->_root->childs_number; ++i){
      int index,index2;
      int num;
      index=i;
      RBTree<int>::Node *node=rb->by_index(index).n;
      if(node==nullptr){
          std::cerr<<"err out of index"<<std::endl;
          std::cin>>index;
      }

      num=node->value;
      rb->search(num,index2);
      std::cout<<index<<" >> "<<num<<" >> "<<index2<<std::endl;
      if(index!=index2){
          std::cin>>index;
      }
  }
    rb->_root->parent=nullptr;
  int c=0;
  for(RBTree<int>::my_iterator i=rb->begin(); i.n!=nullptr; ++i,++c){
      int index2;
      rb->search(i.n->value,index2);
      std::cout<<c<<" :: "<<i.n->value<<" :: "<<index2<<std::endl;
      if(c!=index2){
          std::cin>>c;
      }
  }


  do{
    std::cout << "Enter an integer." << std::endl;
    int index,index2;
    int num;
    std::cin >> index;
    RBTree<int>::Node *node=rb->by_index(index).n;
    if(node==nullptr){
        std::cerr<<"out of index"<<std::endl;
        continue;
    }

    num=node->value;
    rb->search(num,index2);
    std::cout<<index<<" >> "<<num<<" >> "<<index2<<std::endl;
  }while(true);


  delete rb;

  return 0;
}/**/
