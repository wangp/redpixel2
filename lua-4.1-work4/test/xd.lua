-- hex dump

local offset=0

while 1 do
 local s=read(16)
 if s==nil then return end
 write(format("%08X  ",offset))
 gsub(s,"(.)",function (c) write(format("%02X ",strbyte(c))) end)
 write(strrep(" ",3*(16-strlen(s))))
 write(" ",gsub(s,"%c","."),"\n") 
 offset=offset+16
end
