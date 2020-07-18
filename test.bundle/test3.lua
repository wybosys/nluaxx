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