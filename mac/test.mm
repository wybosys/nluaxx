#import "test.h"
#import <nlua/nlua++.hpp>
#import <cross/cross.hpp>
#import <cross/sys.hpp>
#import <cross/fs.hpp>
#import <cross/threads.hpp>
#import <cross/stringbuilder.hpp>
#import <cross/timer.hpp>
#import <cross/datetime.hpp>

USE_NNT;
USE_CROSS;
USE_NLUA;

#define UNITTEST_CHECK_EQUAL(a, b) assert(a == b)
#define CHECK_EQUAL(a, b) assert(a == b)

void test_main()
{
    //::std::mutex mtx;

    // 测试原始lua
    auto &ctx = Context::shared();

    ctx.load("main.lua");
    ctx.invoke("main");
}

void test_test0()
{
    // 测试c++定义lua类，以及从lua调用c++
    auto &ctx = Context::shared();

    auto module = make_shared<Module>();
    module->name = "test";

    auto clz = make_shared<nlua::Class>();
    clz->name = "Test";
    clz->add_field("abc", (integer)100);
    clz->add("proc", [=](self_type &self) -> return_type
    {
        integer abc = self->get("abc")->toInteger();
        UNITTEST_CHECK_EQUAL(abc, 100);
        self->set("abc", (integer)123);
        return make_shared<Variant>("c++");
    });

    clz->add_static("sproc", [=](Variant const &v) -> return_type
    {
        return make_shared<Variant>(v);
    });

    auto clz2 = make_shared<nlua::Class>();
    clz2->name = "Test2";
    // 继承C++构造的lua类
    clz2->inherit(clz);

    module->add(clz);
    module->add(clz2);

    // 第二层module
    auto m2 = make_shared<Module>();
    // m2->name = "test.abc"; 不支持
    m2->name = "abc";

    auto clz22 = make_shared<nlua::Class>();
    clz22->name = "Abc";
    clz22->add("proc", [=](self_type &) -> return_type
    {
        return _V("abc");
    });
    m2->add(clz22);
    module->add(m2);

    module->declare_in(ctx);
    // 重复declare, 应该被跳过
    module->declare_in(ctx);

    ctx.load("test0.lua");
    ctx.invoke("test0");

    auto clz3 = make_shared<nlua::Class>();
    clz3->name = "Test3";
    // 集成lua中声明的全局类
    clz3->inherit(ctx.global("TestAbc"));
    clz3->inherit(ctx.global("TestCde"));
    clz3->declare_in(ctx, *module);

    ctx.add(module);
    ctx.invoke("test0_a");
}

void test_test1()
{
    // 测试 c++ 变量
    auto &ctx = Context::shared();

    {
        auto module = make_shared<Module>();
        module->name = "test";

        auto clz = make_shared<nlua::Class>();
        clz->name = "Test";

        clz->add(make_shared<Field>("a", "a"));
        clz->add(make_shared<Field>("sa", "sa"));
        clz->add(make_shared<Field>("b", nullptr));

        /*
        clz->add("b", [=](self_type &self) -> return_type {
            return nullptr;
        });
         */

        clz->add("proc", [=](self_type &self, Variant const &v) -> return_type
        {
            if (v.isnil())
                NLUA_INFO("收到空参数");
            return make_shared<Variant>(v);
        });

        clz->add("done", [=](self_type &self) -> return_type
        {
                CHECK_EQUAL(self->get("a")->toInteger(), 123);
                CHECK_EQUAL(self->invoke("b")->toInteger(), 123);
                CHECK_EQUAL(self->invoke("proc", 123L)->toInteger(), 123);
                CHECK_EQUAL(self->has("xxxxxxxx"), false);
            return nullptr;
        });

        module->add(clz);
        ctx.add(module); // 如果使用declare_in, 则需要保证将module加入ctx,否则生命期结束后会被释放
        // module->declare_in(ctx);
    }

    ctx.declare(); // 可以独立declarein也可以通过ctx一次性declare

    ctx.load("test1.lua");
    ctx.invoke("test1");
}

void test_test2()
{
    // 测试c++调lua函数
    auto &ctx = Context::shared();

    ctx.load("test2.lua");
    ctx.invoke("test2");

    auto gs_test = ctx.global("gs_test");
        CHECK_EQUAL(gs_test->invoke("proc")->toString(), "nlua++");
    gs_test->set("msg", 123L);
        CHECK_EQUAL(gs_test->get("msg")->toInteger(), 123);
        CHECK_EQUAL(gs_test->invoke("proc")->toInteger(), 123);

    auto Test = ctx.global("Test");
        CHECK_EQUAL(Test->invoke("sproc")->toString(), "lua");
}

void test_test3()
{
    // 定义 lua 的单件
    auto &ctx = Context::shared();

    auto clz = make_shared<nlua::Class>();
    clz->name = "Test";
    clz->singleton("shared", [=](self_type &self)
    {
        NLUA_INFO("初始化");
        self->set("abc", (integer) 123);
    }, [=](self_type &self)
                   {
                           CHECK_EQUAL(self->get("abc")->toInteger(), 123);
                       NLUA_INFO("析构");
                   });
    clz->add("proc", [=, &ctx](self_type &self) -> return_type
    {
        self->set("abc", (integer) 456);
        // 拿静态对象
        auto obj = ctx.singleton("Test");
            CHECK_EQUAL(obj->get("abc")->toInteger(), 456);
        return nullptr;
    });
    clz->declare_in(ctx);

    ctx.load("test3.lua");
    ctx.invoke("test3");
}

int test4_a()
{
    return 0;
}

void test_test4()
{
    auto &ctx = Context::shared();

    ctx.load("test4.lua");

    auto clz = make_shared<nlua::Class>();
    clz->name = "Test";
    clz->add_static("proc", [=]() -> return_type
    {
        return nullptr;
    });
    clz->declare_in(ctx);

    auto Test = ctx.global("Test");

    // 测试耗时
    TimeCounter tc;
    tc.start();

    integer cnt = 100000;

    NLUA_INFO("开始测试lua函数执行 " << cnt << " 次");
    ctx.invoke("test4_a", cnt);
    NLUA_INFO("共耗时 " << tc.seconds() << " s");

    NLUA_INFO("开始测试c++调用lua函数 " << cnt << " 次");
    for (int i = 0; i < cnt; ++i) {
        ctx.invoke("test4");
    }
    NLUA_INFO("共耗时 " << tc.seconds() << " s");

    NLUA_INFO("开始测试c++调用 " << cnt << " 次");
    for (int i = 0; i < cnt; ++i) {
        test4_a();
    }
    NLUA_INFO("共耗时 " << tc.seconds() << " s");

    NLUA_INFO("开始测试c++调用c++实现的lua函数 " << cnt << " 次");
    for (int i = 0; i < cnt; ++i) {
        Test->invoke("proc");
    }
    NLUA_INFO("共耗时 " << tc.seconds() << " s");
}

void test_test5()
{
    // 测试 ss
    auto &ctx = Context::shared();
    
    ctx.load("test5.lua");
    ctx.invoke("test5");
}

void test_test6()
{
    auto &ctx = Context::shared();
    
    auto m = make_shared<Module>();
    m->name = "test";
    
    auto clz = make_shared<nlua::Class>();
    clz->name = "Test";
    clz->add(make_shared<Field>("ondone", nullptr));
    clz->add_field("onend", nullptr);
    clz->add("play", [&](self_type &self, arg_type const &msg) -> return_type
             {
        NLUA_AUTOGUARD(ctx);
        
        // 保护变量，避免被局部释放
        self->grab();
        
        self->invoke("ondone");
        self->invoke("onend");
        
        // 稳定重现多线程写冲突，同一个变量被多个线程并发写数据
        auto s = ctx.global("test.Test")->instance();
        s->grab();
        
        for (int i = 0; i < 1000; ++i) {
            s->set("a", (integer) i);
        }
        NLUA_DEBUG("快速写a完成");
        
        // 测试长生命周期异步调用
        Timer::SetTimeout(0.1, [=, &ctx]()
                          {
            NLUA_AUTOGUARD(ctx);
            
            // onend实现，ondone未实现
            self->invoke("ondone");
            self->invoke("onend");
            
            for (int i = 0; i < 1000; ++i) {
                s->set("b", (integer) i);
                Time::Sleep(0.001);
            }
            NLUA_DEBUG("写b完成");
            
            Timer::SetTimeout(0.1, [=, &ctx]()
                              {
                NLUA_AUTOGUARD(ctx);
                
                for (int i = 0; i < 1000; ++i) {
                    s->set("c", (integer) i);
                    Time::Sleep(0.001);
                }
                NLUA_DEBUG("写c完成");
                
                // 因为已经drop不应打印出任何东西
                self->invoke("ondone");
                self->invoke("onend");
                
                self->drop(); // 重复drop但是可以不引起崩溃
            });
            
            self->drop();
        });
        
        for (int i = 0; i < 1000; ++i) {
            s->set("a", (integer) i);
            Time::Sleep(0.001);
        }
        NLUA_DEBUG("写a完成");
        
        NLUA_INFO(msg);
        return nullptr;
    });
    
    // 实现类
    m->add(clz);
    m->declare_in(ctx);
    
    ctx.add(m);
    
    // 启动
    ctx.load("test6.lua");
    ctx.invoke("test6");
}

class Test7Object
{
public:
    Test7Object()
    {
        NLUA_INFO("实例化 Test7Object");
    }

    ~Test7Object()
    {
        NLUA_INFO("析构 Test7Object");
    }

    string txt;
};

void test_test7()
{
    // 测试c++生命期绑定至lua对象
     auto &ctx = Context::shared();
     NLUA_AUTOGUARD(ctx);

     auto clz = make_shared<nlua::Class>();
     clz->name = "Test";

     clz->init([=](self_type &self, Variant const &v0, Variant const &v1)
               {
                       CHECK_EQUAL(v0.toInteger(), 1);
                       CHECK_EQUAL(v1.toInteger(), 2);
                   self->bind<Test7Object>().txt = "test7";
               });
     clz->fini([=](self_type &self)
               {
                   self->unbind<Test7Object>();
               });
     clz->add("proc", [=](self_type &self) -> return_type
     {
         return make_shared<Variant>(self->payload<Test7Object>().txt);
     });
     clz->add("proc2", [=](self_type &self) -> return_type
     {
         return self->get("abc");
     });

     auto m = make_shared<Module>();
     m->name = "test";
     m->add(clz);

     m->declare_in(ctx);
     ctx.add(m);

     ctx.load("test7.lua");
     ctx.invoke("test7");
}

void test_test8()
{
    //c++ 实例化lua对象，并讲c++生命期绑定到lua对象上
    auto &ctx = Context::shared();
    NLUA_AUTOGUARD(ctx);

    auto clz = make_shared<nlua::Class>();
    clz->name = "Test";

    clz->add_static("proc", [&]() -> return_type
    {
        auto abc = ctx.global("abc.Abc");
        auto r = abc->instance();
            CHECK_EQUAL(r->invoke("proc")->toString(), "abc");
        return r->toVariant();
    });

    auto m = make_shared<Module>();
    m->name = "test";
    m->add(clz);

    m->declare_in(ctx);
    ctx.add(m);

    ctx.load("test8.lua");
    ctx.invoke("test8");
}

void test_test999()
{
    return; // 协程测试会阻塞单元测试流程
    // 测试协程
    auto &ctx = Context::shared();
    
    ctx.load("test999.lua");
    ctx.invoke("test999");
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
    test_test0();
    test_test1();
    test_test2();
    test_test3();
    test_test4();
    test_test5();
    test_test6();
    test_test7();
    test_test8();
    test_test999();
    
    MainThread::shared().exec();
}
