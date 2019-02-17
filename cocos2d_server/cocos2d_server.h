#ifndef COCOS2D_SERVER_H
#define COCOS2D_SERVER_H

#ifdef SERVER_SIDE
   #include "CocoBase.h"
    #include <iostream>
    #define _show(x)  std::cerr<<#x<<" "<<x<<std::endl;

#else
    #include "cocos2d.h"
    using namespace cocos2d;
    #define _show(x);
#endif
using namespace std;
#endif // COCOS2D_SERVER_H
