-- decode quoted-printable text

T=io.read"*a"
T=string.gsub(T,"=\n","")
T=string.gsub(T,"=(%x%x)",function (x) return string.char(tonumber(x,16)) end)
io.write(T)
