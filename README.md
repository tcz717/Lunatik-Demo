# Lunatik - Demo

Author: Chengzhi Tan

## "Hello World" Kernel Module

Source Code Location: `helloworld_kernel/src/helloworld.c`

### How to use

```shell
make
sudo insmod helloworld_kernel/src/helloworld.ko
```

## UDP Socket Lua Library

Source Code Location: `luaudp/src/luaudp.c`

### Build

```shell
mkdir build && cd build
cmake ../luaudp/
make
# unit test
ctest
```

### Example

```lua
local luaudp = require("libluaudp")

writer = luaudp.new()
reader = luaudp.new()
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
```