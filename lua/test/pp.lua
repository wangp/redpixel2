-- implementation of Lua 3.x preprocessor

local function abort(name,line,why)
 error(name..":"..line..": "..why.."\n",0)
end

local function eval(s,name,line)
 local f,e=loadstring("return "..s,"=$if")
 if f==nil then abort(name,line,e) end
 return f()
end

local function scan(IN,OUT,name,ln,fp,pp,p)
 local e=nil				-- no `else' seen
 p=pp and p				-- ensure consistency of print flags
 while 1 do
  local s=IN:read()
  if s==nil then
   if fp then return else abort(name,ln,"unexpected end of file") end
  end
  ln=ln+1
  local i,j,c=string.find(s,"%$%s*(%w+)")
  if not i then				-- ordinary line
   if not p then s="" end		-- preserve line numbers
   OUT:write(s,"\n")
  else					-- preprocessor line
   OUT:write("\n")			-- preserve line numbers
   local t=string.sub(s,j+1,-1)
   if c=="debug" then			-- do nothing
   elseif c=="nodebug" then		-- do nothing
   elseif c=="endinput" then return 0
   elseif c=="if" then
    local v=eval(t,name,ln)
    ln=scan(IN,OUT,name,ln,nil,p,v)
    if ln==0 then return 0 end
   elseif c=="ifnot" then
    local v=eval(t,name,ln)
    ln=scan(IN,OUT,name,ln,nil,p,not v)
    if ln==0 then return 0 end
   elseif c=="end" then
    if fp then
     abort(name,ln,"unmatched `endif'")
    else
     return ln
    end
   elseif c=="else" then
    if fp then
     abort(name,ln,"unmatched `else'")
    elseif e then
     abort(name,ln,"extraneous `else'")
    else
     p=pp and not p			-- invert print permission
     e=1				-- `else' seen
    end
   else
    abort(name,ln,"unknown pragma `"..c.."'")
   end
  end
 end
end

function pploadfile(s)
 local IN,e=io.open(s,"r")	assert(IN,e)
 local t=os.tmpname()		assert(t)
 local OUT,e=io.open(t,"w")	assert(OUT,e)
 scan(IN,OUT,s,0,1,1,1)
 io.close(OUT)
 local a,b=loadfile(t,s)
 os.remove(t)
 if a==nil then
  a=string.gsub(b,".-:([0-9]+): ",function (x) b=x return "" end)
  a=string.gsub(a,"\n","")
  abort(s,b,a)
 end
 return a,b
end

-- an example
-- pploadfile("i")()
