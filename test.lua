function main()
    local cls = {
        a = "a",
        sa = "sa",
    }

    local t = {}
    setmetatable(t, { __index = cls })

    cls.a = 123
    assert(t.a == 123, "obj a 123")

    return 0
end

function test0()
    local t = test.Test:new()
    assert(t:proc() == "c++", "proc failed")
    assert(t:sproc(123) == 123, "sproc failed")
    assert(test.Test:sproc("abc") == "abc", "sproc failed")

    t = test.Test2:new()
    assert(t:proc() == "c++", "proc failed")
    assert(t:sproc(123) == 123, "sproc failed")
    assert(test.Test:sproc("abc") == "abc", "sproc failed")

    TestAbc = { a = 1 }
    function TestAbc:proc()
        return "abc"
    end

    TestCde = { b = 2 }
    function TestCde:proc1()
        return "cde"
    end

    return 0
end

function test0_a()
    local t = test.Test3:new()
    -- local abc = TestAbc:new()
    assert(t:proc() == "abc", "test3 abc")
    assert(t:proc1() == "cde", "test3 cde")
    assert(t.a == 1, "test3 a 1")
end

function test1()
    assert(test.Test.a == "a", "global a")
    assert(test.Test.sa == "sa", "global sa")
    local t0 = test.Test:new()
    local t1 = test.Test:new()
    assert(t0.a == "a", "obj0 a")
    assert(t0.sa == "sa", "obj0 sa")
    t0.a = 123
    assert(t0.a == 123, "obj0 a 123")
    assert(t1.a == "a", "obj1 a a")
    test.Test.a = 456
    assert(t0.a == 123, "obj0 a 456")
    assert(t1.a == 456, "obj1 a 456")
    test.Test.sa = 123
    assert(t1.sa == 123, "obj1 sa 123")
    t0:done()
    return 0
end

function test2()
    Test = { msg = "lua" }
    assert(_G["Test"] == Test, "global class")

    function Test:proc()
        return self.msg
    end

    function Test:sproc()
        return Test.msg
    end

    gs_test = {}
    setmetatable(gs_test, { __index = Test })
    assert(_G["gs_test"] == gs_test, "global variable")

    -- print(gs_test:proc())
    gs_test.msg = "nlua++"
    -- print(gs_test:proc())

    return 0
end

function test3()
    assert(Test.new == nil, "shared has new")
    t0 = Test:shared()
    t1 = Test:shared()
    -- print(t0)
    -- print(t1)
    assert(t0 == t1, "shared equal")
    Test:free_shared()
    t1 = Test:shared()
    -- print(t0)
    -- print(t1)
    assert(t1:proc() == Test:shared():proc(), "two shared unequal")
    assert(t0 ~= t1, "shared unequal")
    return 0
end
