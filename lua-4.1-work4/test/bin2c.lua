-- bare-bones bin2c in Lua

write"static const char B[]=\n"
local n=0
while 1 do
 local c=read(1)
 if c==nil then break end
 if n==0 then write '"' n=0 end
 write(format("\\%03o",strbyte(c)))
 n=n+1
 if n==19 then write '"\n' n=0 end
end
if n~=0 then write '";\n' n=0 end
write[[

lua_dobuffer(L,B,sizeof(B),0);
]]
