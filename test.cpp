#include "nlua++.h"
#include <UnitTest++/UnitTest++.h>
#include <UnitTest++/TestReporterStdout.h>
#include "timer.h"

USE_NLUA

TEST (main) {
    // 测试原始lua
    Context ctx;
    ctx.add_package_path("../nlua");

    ctx.load("test.lua");
    ctx.invoke("main");
}

TEST (test0) {
    // 测试c++定义lua类，以及从lua调用c++
    Context ctx;
    ctx.add_package_path("../nlua");

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
    module->declare_in(ctx);

    ctx.load("test.lua");
    ctx.invoke("test0");

    auto clz3 = make_shared<Class>();
    clz3->name = "Test3";
    // 集成lua中声明的全局类
    clz3->inherit(ctx.global("TestAbc"));
    clz3->inherit(ctx.global("TestCde"));
    clz3->declare_in(ctx, *module);

    ctx.invoke("test0_a");
}

TEST (test1) {
    // 测试 c++ 变量
    Context ctx;
    ctx.add_package_path("/../nlua");

    auto module = make_shared<Module>();
    module->name = "test";

    auto clz = make_shared<Class>();
    clz->name = "Test";

    clz->add(make_shared<Field>("a", "a"));
    clz->add(make_shared<Field>("sa", "sa"));

    clz->add("proc", [=](self_type &self, Variant const &v) -> return_type {
        return make_shared<Variant>(v);
    });

    clz->add("done", [=](self_type &self) -> return_type {
                CHECK_EQUAL(self->get("a")->toInteger(), 123);
                CHECK_EQUAL(self->invoke("proc", 123)->toInteger(), 123);
                CHECK_EQUAL(self->has("xxxxxxxx"), false);
        return nullptr;
    });

    module->add(clz);
    module->declare_in(ctx);

    ctx.load("test.lua");
    ctx.invoke("test1");
}

TEST (test2) {
    // 测试c++调lua函数
    Context ctx;
    ctx.add_package_path("../nlua");

    ctx.load("test.lua");
    ctx.invoke("test2");

    auto gs_test = ctx.global("gs_test");
            CHECK_EQUAL(gs_test->invoke("proc")->toString(), "nlua++");
    gs_test->set("msg", 123);
            CHECK_EQUAL(gs_test->get("msg")->toInteger(), 123);
            CHECK_EQUAL(gs_test->invoke("proc")->toInteger(), 123);

    auto Test = ctx.global("Test");
            CHECK_EQUAL(Test->invoke("sproc")->toString(), "lua");
}

TEST (test3) {
    // 定义 lua 的单件
    Context ctx;
    ctx.add_package_path("../nlua");

    auto clz = make_shared<Class>();
    clz->name = "Test";
    clz->singleton("shared", [=]() {
        // cout << "初始化" << endl;
    }, [=]() {
        // cout << "析构" << endl;
    });
    clz->add("proc", [=](self_type &self) -> return_type {
        return make_shared<Variant>(self->pointer());
    });
    clz->declare_in(ctx);

    ctx.load("test.lua");
    ctx.invoke("test3");
}

int test4_a() {
    return 0;
}

TEST (test4) {
    Context ctx;
    ctx.add_package_path("../nlua");

    ctx.load("test.lua");

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

    int cnt = 100000;

    cout << "开始测试lua函数执行 " << cnt << " 次" << endl;
    ctx.invoke("test4_a");
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

int main() {
    ::UnitTest::TestReporterStdout rpt;
    ::UnitTest::TestRunner runner(rpt);
    runner.RunTestsIf(::UnitTest::Test::GetTestList(), nullptr, ::UnitTest::True(), 0);
    return Exec();
}

