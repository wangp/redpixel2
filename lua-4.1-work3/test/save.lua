-- save tables as Lua code
-- with contributions from Jon Kleiser <jon.kleiser@usit.uio.no>

local VISITED={REF={}}

function save1(n,v)
  local t=type(v)
  if t=="nil" or t=="userdata" or t=="function" then return end
  write("T.",n,"=")
  if t=="string" then
   write(format("%q",v))
  elseif t=="number" then
   write(v)
  elseif t=="table" then
   if %VISITED.REF[v] then
    write("T.",%VISITED.REF[v])
   else
    write("{}\n")
    %VISITED.REF[v] = n
    for r,f in v do
     if type(r) == 'string' then
      save1(n.."."..r,f)
     else
      save1(n.."["..r.."]",f)
     end
    end
   end
  end
  write("\n")
end

function save (t,m)
  if writeto(m) then
   write("-- begin ",m,"\n\nlocal T={}\n")
   %VISITED.REF={}
   foreach(t,save1)
   write("\nreturn T\n\n-- end ",m,"\n")
   writeto()
  end
end

-- an example

a = 3
x = {a = 4, b = "name", c = 'quoted "name" end', l={4,5,67}}

b = {t=5,L=x.l}
x.next = b

save(globals(),"global environment")
save(x,"my table")
