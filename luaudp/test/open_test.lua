local luaudp = require("libluaudp")
assert(luaudp)

-- test open function
socket = luaudp.new()
assert(not pcall(socket.bind))
assert(not pcall(socket.bind, 1, 1))
assert(not pcall(socket.bind, "1", 1))
assert(not pcall(socket.bind, socket, "1", 1))
assert(pcall(socket.bind, socket, "127.0.0.1", 55444))

-- test object binding

socket = luaudp.new()
assert(
    pcall(
        function()
            socket:bind("127.0.0.1", 55445)
        end
    )
)
