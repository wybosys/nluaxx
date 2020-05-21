function test7_a()
    local t = test.Test:new(1, 2)
    assert(t:proc() == "test7", "test7 proc is test7")
    -- t = nil 没有用，主要靠 gc
    t:delete()
end

function test7()
    test7_a()
    collectgarbage("collect")
end
