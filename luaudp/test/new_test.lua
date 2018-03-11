local luaudp = require("libluaudp")
assert(luaudp)

-- test new function
assert(pcall(luaudp.new))
