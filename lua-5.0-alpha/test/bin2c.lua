-- bare-bones bin2c in Lua
-- usage: lua bin2c.lua < file

io.write"static const char B[]=\n"
local n=0
while 1 do
 local c=io.read(1)
 if c==nil then break end
 if n==0 then io.write '"' n=0 end
 io.write(string.format("\\%03o",string.byte(c)))
 n=n+1
 if n==19 then io.write '"\n' n=0 end
end
if n~=0 then io.write '";\n' n=0 end
io.write[[

lua_dobuffer(L,B,sizeof(B),0);
]]
