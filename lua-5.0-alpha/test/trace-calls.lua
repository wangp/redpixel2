-- trace calls
-- example: lua -ltrace-calls.lua bisect.lua

local level=0

function hook(event)
--print(debug.traceback())
-- exit()
 local t=debug.getinfo(2)
 io.write(">>> ",string.rep(" ",level))
 t=debug.getinfo(3)
 if t~=nil and t.currentline>=0 then io.write(t.short_src,":",t.currentline," ") end
--if t~=nil then table.foreach(t,print) end
 t=debug.getinfo(2)
 if event=="call" then level=level+1 else level=level-1 end
 if t.what=="main" then
  if event=="call" then
   io.write("begin ",t.short_src)
  else
   io.write("end ",t.short_src)
  end
 elseif t.what=="Lua" then
  io.write(event," ",t.name," <",t.linedefined,":",t.short_src,">")
 else
  io.write(event," ",t.name," [",t.what,"] ")
 end
 io.write("\n")
end

debug.sethook(hook,"cr")
