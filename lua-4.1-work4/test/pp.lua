-- implementation of Lua 3.x preprocessor

function scan(IN,out,name,ln,fp,pp,p)
 local e=nil				-- no `else' seen
 p=pp and p				-- ensure consistency of print flags
 while 1 do
  local s=read(IN)
  if s==nil then
   if fp then return else error("unexpected end of file `"..name.."'") end
  end
  ln=ln+1
  local i,j,c=strfind(s,"%$%s*(%w+)")
  if not i then				-- ordinary line
   if not p then s="" end		-- preserve line numbers
   write(out,s,"\n")
  else					-- preprocessor line
   write(out,"\n")			-- preserve line numbers
   local t=strsub(s,j+1,-1)
   if c=="debug" then			-- do nothing
   elseif c=="nodebug" then		-- do nothing
   elseif c=="endinput" then return 0
   elseif c=="if" then
    local v=dostring("return "..t)
    ln=scan(IN,out,name,ln,nil,p,v)
    if ln==0 then return 0 end
   elseif c=="ifnot" then
    local v=dostring("return "..t)
    ln=scan(IN,out,name,ln,nil,p,not v)
    if ln==0 then return 0 end
   elseif c=="end" then
    if fp then
     error("unmatched `endif' at line "..ln.." of file `"..name.."'")
    else
     return ln
    end
   elseif c=="else" then
    if fp then
     error("unmatched `else' at line "..ln.." of file `"..name.."'")
    elseif e then
     error("extraneous `else' at line "..ln.." of file `"..name.."'")
    else
     p=pp and not p			-- invert print permission
     e=1				-- `else' seen
    end
   else
    error("unknown pragma `"..c.."' at line "..ln.." of file `"..name.."'")
   end
  end
 end
end

function dofile(s)
 local IN,e=openfile(s,"r")	assert(IN,e)
 local t=tmpname()		assert(t)
 local out,e=openfile(t,"w")	assert(out,e)
 scan(IN,out,s,0,1,1,1)
 closefile(out)
 local a1,a2,a3,a4,a5,a6,a7,a8,a9=%dofile(t,s)		-- lazy!
 remove(t)
 return a1,a2,a3,a4,a5,a6,a7,a8,a9
end
