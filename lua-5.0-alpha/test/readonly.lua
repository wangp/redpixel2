-- make global variables readonly

local f=function (t,i) error("cannot redefine global variable `"..i.."'",2) end
local g={}
local G=getglobals()
setmetatable(g,{__index=G,__newindex=f})
setglobals(1,g)

-- an example
rawset(g,"x",3)
x=2
y=1
