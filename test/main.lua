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

