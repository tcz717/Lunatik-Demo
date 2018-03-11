local luaudp = require("libluaudp")
assert(luaudp)

writer = luaudp.new()
reader = luaudp.new()
assert(writer)
assert(reader)
writer:bind("127.0.0.1", 55566)
reader:bind("0.0.0.0", 55588)

function FastBytesToString(bytes)
    s = {}
    for i = 1, #bytes do
        s[i] = string.char(bytes[i])
    end
    return table.concat(s)
end

writer:write({string.byte("hello world", 1, -1)}, "127.0.0.1", 55588)
data, ip, port = reader:read(20)
assert(FastBytesToString(data) == "hello world")
print(ip)
assert(ip == "127.0.0.1")
print(port)
assert(port == 55566)

writer:close()
reader:close()
writer:close()
reader:close()
