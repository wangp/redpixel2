-- list header contents of precompiled chunks

function byte() return strbyte(read(1)) end

-- signature
s=read(4)
if s~="\027Lua" then error"not a Lua precompiled chunk" end

-- version
b=byte()
write("Lua ",floor(b/16),".",mod(b,16),"\n")

-- endianness
b=byte()
if b==1 then write"little endian\n" else write"big endian\n" end

-- vm spec
write(byte(),"-byte integers\n")
write(byte(),"-byte size_t\n")
write(byte(),"-byte Instruction: ")
write(byte(),"-bit OP, ")
write(byte(),"-bit A, ")
write(byte(),"-bit B, ")
write(byte(),"-bit C\n")
write(byte(),"-byte numbers\n")
