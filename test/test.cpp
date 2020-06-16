#include "nlua++.hpp"
#include <UnitTest++/UnitTest++.h>
#include <UnitTest++/TestReporterStdout.h>

#include <cross/cross.hpp>
#include <cross/timer.hpp>
#include <cross/threads.hpp>

USE_NLUA;
USE_CROSS;

TEST (main) {
    // 测试原始lua
    auto &ctx = Context::shared();

    ctx.load("main.lua");
    ctx.invoke("main");
}

TEST (test0) {
    // 测试c++定义lua类，以及从lua调用c++
    auto &ctx = Context::shared();

    auto module = make_shared<Module>();
    module->name = "test";

    auto clz = make_shared<Class>();
    clz->name = "Test";

    clz->add("proc", [=](self_type &self) -> return_type {
        return make_shared<Variant>("c++");
    });

    clz->add_static("sproc", [=](Variant const &v) -> return_type {
        return make_shared<Variant>(v);
    });

    auto clz2 = make_shared<Class>();
    clz2->name = "Test2";
    // 继承C++构造的lua类
    clz2->inherit(clz);

    module->add(clz);
    module->add(clz2);

    // 第二层module
    auto m2 = make_shared<Module>();
    // m2->name = "test.abc"; 不支持
    m2->name = "abc";

    auto clz22 = make_shared<Class>();
    clz22->name = "Abc";
    clz22->add("proc", [=](self_type&)->return_type {
        return _V("abc");
        });
    m2->add(clz22);
    module->add(m2);

    module->declare_in(ctx);
    // 重复declare, 应该被跳过
    module->declare_in(ctx);

    ctx.load("test0.lua");
    ctx.invoke("test0");

    auto clz3 = make_shared<Class>();
    clz3->name = "Test3";
    // 集成lua中声明的全局类
    clz3->inherit(ctx.global("TestAbc"));
    clz3->inherit(ctx.global("TestCde"));
    clz3->declare_in(ctx, *module);

    ctx.add(module);
    ctx.invoke("test0_a");
}

TEST (test1) {
    // 测试 c++ 变量
    auto &ctx = Context::shared();

    {
        auto module = make_shared<Module>();
        module->name = "test";

        auto clz = make_shared<Class>();
        clz->name = "Test";

        clz->add(make_shared<Field>("a", "a"));
        clz->add(make_shared<Field>("sa", "sa"));
        clz->add(make_shared<Field>("b", nullptr));

        /*
        clz->add("b", [=](self_type &self) -> return_type {
            return nullptr;
        });
         */

        clz->add("proc", [=](self_type &self, Variant const &v) -> return_type {
            if (v.isnil())
                cout << "收到空参数" << endl;
            return make_shared<Variant>(v);
        });

        clz->add("done", [=](self_type &self) -> return_type {
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

TEST (test2) {
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

TEST (test3) {
    // 定义 lua 的单件
    auto &ctx = Context::shared();

    auto clz = make_shared<Class>();
    clz->name = "Test";
    clz->singleton("shared", [=](self_type& self) {
        cout << "初始化" << endl;
        self->set("abc", 123);
    }, [=](self_type& self) {
        CHECK_EQUAL(self->get("abc")->toInteger(), 123);
        cout << "析构" << endl;
    });
    clz->add("proc", [=, &ctx](self_type &self) -> return_type {
        self->set("abc", 456);
        // 拿静态对象
        auto obj = ctx.singleton("Test");
        CHECK_EQUAL(obj->get("abc")->toInteger(), 456);
        return nullptr;
    });
    clz->declare_in(ctx);

    ctx.load("test3.lua");
    ctx.invoke("test3");
}

int test4_a() {
    return 0;
}

TEST (test4) {
    auto &ctx = Context::shared();

    ctx.load("test4.lua");

    auto clz = make_shared<Class>();
    clz->name = "Test";
    clz->add_static("proc", [=]() -> return_type {
        return nullptr;
    });
    clz->declare_in(ctx);

    auto Test = ctx.global("Test");

    // 测试耗时
    TimeCounter tc;
    tc.start();

    integer cnt = 100000;

    cout << "开始测试lua函数执行 " << cnt << " 次" << endl;
    ctx.invoke("test4_a", cnt);
    cout << "共耗时 " << tc.seconds() << " s" << endl;

    cout << "开始测试c++调用lua函数 " << cnt << " 次" << endl;
    for (int i = 0; i < cnt; ++i) {
        ctx.invoke("test4");
    }
    cout << "共耗时 " << tc.seconds() << " s" << endl;

    cout << "开始测试c++调用 " << cnt << " 次" << endl;
    for (int i = 0; i < cnt; ++i) {
        test4_a();
    }
    cout << "共耗时 " << tc.seconds() << " s" << endl;

    cout << "开始测试c++调用c++实现的lua函数 " << cnt << " 次" << endl;
    for (int i = 0; i < cnt; ++i) {
        Test->invoke("proc");
    }
    cout << "共耗时 " << tc.seconds() << " s" << endl;
}

TEST (test5) {
    // 测试 ss
    auto &ctx = Context::shared();

    ctx.load("test5.lua");
    ctx.invoke("test5");
}

TEST (test6) {
    auto &ctx = Context::shared();

    auto m = make_shared<Module>();
    m->name = "test";

    auto clz = make_shared<Class>();
    clz->name = "Test";
    clz->add(make_shared<Field>("ondone", nullptr));
    clz->add_field("onend", nullptr);
    clz->add("play", [&](self_type &self, Variant const &msg) -> return_type {
        // 保护变量，避免被局部释放
        self->grab();

        // 测试长生命周期异步调用
        Timer::SetTimeout(1, [=]() {
            // onend实现，ondone未实现
            self->invoke("ondone");
            self->invoke("onend");

            Timer::SetTimeout(1, [=]() {
                // 因为已经drop不应打印出任何东西
                self->invoke("ondone");
                self->invoke("onend");

                self->drop(); // 重复drop但是可以不引起崩溃
                });

            self->drop();
        });

        cout << msg << endl;
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

class Test7Object {
public:
    Test7Object() {
        cout << "实例化 Test7Object" << endl;
    }

    ~Test7Object() {
        cout << "析构 Test7Object" << endl;
    }

    string txt;
};

TEST (test7) {
    // 测试c++生命期绑定至lua对象
    auto &ctx = Context::shared();

    auto clz = make_shared<Class>();
    clz->name = "Test";

    clz->init([=](self_type &self, Variant const &v0, Variant const &v1) {
                CHECK_EQUAL(v0.toInteger(), 1);
                CHECK_EQUAL(v1.toInteger(), 2);
        self->bind<Test7Object>().txt = "test7";
    });
    clz->fini([=](self_type &self) {
        self->unbind<Test7Object>();
    });
    clz->add("proc", [=](self_type &self) -> return_type {
        return make_shared<Variant>(self->payload<Test7Object>().txt);
    });
    clz->add("proc2", [=](self_type &self)->return_type {
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

TEST(test8)
{
    //c++ 实例化lua对象，并讲c++生命期绑定到lua对象上
    auto &ctx = Context::shared();

    auto clz = make_shared<Class>();
    clz->name = "Test";

    clz->add_static("proc", [&]()->return_type {
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

TEST (test999) {
    return; // 协程测试会阻塞单元测试流程
    // 测试协程
    auto &ctx = Context::shared();

    ctx.load("test999.lua");
    ctx.invoke("test999");
}

int main() {
    Context::shared().create();
    Context::shared().add_package_path("../test");

    ::UnitTest::TestReporterStdout rpt;
    ::UnitTest::TestRunner runner(rpt);
    runner.RunTestsIf(::UnitTest::Test::GetTestList(), nullptr, ::UnitTest::True(), 0);

    MainThread::shared().exec();
    return 0;
}
