function test7_a()
    local t = test.Test:new(1, 2)
    -- t:delete()
    t = nil
end

function test7()
    test7_a()
    collectgarbage("collect")
end
