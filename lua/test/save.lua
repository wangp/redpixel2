-- save tables as Lua code
-- with contributions from Jon Kleiser <jon.kleiser@usit.uio.no>

local VISITED={REF={}}

function save1(n,v)
  local t=type(v)
  if t=="nil" or t=="userdata" or t=="function" then return end
  io.write("T.",n,"=")
  if t=="string" then
   io.write(string.format("q",v))
  elseif t=="number" then
   io.write(v)
  elseif t=="table" then
   if VISITED.REF[v] then
    io.write("T.",VISITED.REF[v])
   else
    io.write("{}\n")
    VISITED.REF[v] = n
    for r,f in v do
     if type(r) == 'string' then
      save1(n.."."..r,f)
     else
      save1(n.."["..r.."]",f)
     end
    end
   end
  end
  io.write("\n")
end

function save (t,m)
  io.write("-- begin ",m,"\n\nlocal T={}\n")
  VISITED.REF={}
  table.foreach(t,save1)
  io.write("\nreturn T\n\n-- end ",m,"\n")
end

-- an example
a = 3
x = {a = 4, b = "name", c = 'quoted "name" end', l={4,5,67}}

b = {t=5,L=x.l}
x.next = b

--save(getglobals(),"global environment")
save(x,"my table")
