-- list header contents of precompiled chunks
-- typical usage: lua header.lua < luac.out

function byte() return string.byte(io.read(1)) end

-- signature
s=io.read(4)
if s~="\027Lua" then error"stdin is not a Lua precompiled chunk" end

-- version
b=byte()
v=math.floor(b/16).."."..math.mod(b,16)
io.write("Lua ",v,"\n")
v=tonumber(v)
if v<4 then return end

-- endianness
b=byte()
if b==1 then io.write"little endian\n" else io.write"big endian\n" end

-- vm spec
io.write(byte(),"-byte integers\n")
io.write(byte(),"-byte size_t\n")
io.write(byte(),"-byte Instructions: ")
io.write(byte(),"-bit OP, ")
if v>4 then io.write(byte(),"-bit A, ") end
io.write(byte(),"-bit B, ")
if v>4 then io.write(byte(),"-bit C\n") end
io.write(byte(),"-byte numbers\n")
