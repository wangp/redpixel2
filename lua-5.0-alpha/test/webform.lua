-- convert POST data to Lua table

T=io.read"*a"	-- for GET, use T=os.getenv"QUERY_STRING"
T=string.gsub(T,"=","=[[")
T=string.gsub(T,"&","]],\n")
T=string.gsub(T,"+"," ")
T=string.gsub(T,"%%(%x%x)",function (x) return string.char(tonumber(x,16)) end)
io.write("form{\n",T,"]]\n}\n")
