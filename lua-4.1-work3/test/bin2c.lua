-- bare-bones bin2c in Lua

write"static const unsigned char B[]={\n"
local n=0
while 1 do
 local c=read(1)
 if c==nil then break end
 write(format("%3d,",strbyte(c)))
 n=n+1
 if n==20 then write"\n" n=0 end
end
write[[
};

lua_dobuffer(L,B,sizeof(B),0);
]]
