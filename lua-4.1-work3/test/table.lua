-- make table, grouping all data for the same item
-- input is 2 columns (item, data)

local A
while 1 do
 local l=read()
 if l==nil then write("\n"); return end
 local _,_,a,b=strfind(l,'"?(%w+)"?%s*(.*)$')
 if a~=A then A=a write("\n",a,":") end
 write(" ",b)
end
