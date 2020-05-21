function test7_a()
    local t = test.Test:new(1, 2)
    assert(t:proc() == "test7", "test7 proc is test7")
    t = nil
end

function test7()
    test7_a()
    collectgarbage("collect")
end
