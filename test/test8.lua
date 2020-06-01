Abc = {}

function Abc:proc()
    return "abc"
end

function test8()
    test.Test:proc()
    collectgarbage("collect")
end
