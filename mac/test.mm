#import "test.h"
#import <nlua/nlua++.hpp>
#import <cross/cross.hpp>
#import <cross/sys.hpp>
#import <cross/fs.hpp>
#import <cross/threads.hpp>

USE_NNT;
USE_CROSS;

#define UNITTEST_CHECK_EQUAL(a, b) assert(a == b)

void test_main()
{
    
}

void Test()
{
    cd(path_home());
    
    test_main();
    
    MainThread::shared().exec();
}
