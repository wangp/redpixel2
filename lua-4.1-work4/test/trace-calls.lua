-- trace calls
-- example: lua trace-calls.lua hello.lua bisect.lua

function callhook(event)
 local t=getinfo(2)
 write(">>> ")
--foreach(t,print)
 if t.what=="main" then
  if event=="call" then
   write("begin ",t.source)
  else
   write("end ",t.source)
  end
 elseif t.what=="Lua" then
  write(event," ",t.name," <",t.linedefined,":",t.source,">")
 else
  write(event," ",t.name," [",t.what,"] ")
 end
 if t.currentline>=0 then write(":",t.currentline) end
 write("\n")
end

setcallhook(callhook)

function linehook(line)
 local t=getinfo(2)
 write("--- ",line," ",t.source,"\n")
end

setlinehook(linehook)
