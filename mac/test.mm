#import "test.h"
#import <nlua/nlua++.hpp>
#import <cross/cross.hpp>
#import <cross/sys.hpp>
#import <cross/fs.hpp>
#import <cross/threads.hpp>

USE_NNT;
USE_CROSS;
USE_NLUA;

#define UNITTEST_CHECK_EQUAL(a, b) assert(a == b)

void test_main()
{
    //::std::mutex mtx;

    // 测试原始lua
    auto &ctx = Context::shared();

    ctx.load("main.lua");
    ctx.invoke("main");
}

void Test()
{
    Context::shared().create();

#if TARGET_OS_OSX
    string testdir = absolute(dirname(__FILE__) + "/../test.bundle");
#else
    string testdir = path_app() + "/test.bundle";
#endif
    Context::shared().add_package_path(testdir);
    
    cd(path_home());
    
    test_main();
    
    MainThread::shared().exec();
}
