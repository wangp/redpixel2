function generatefib (n)
  return coroutine(function (n)
    local a,b = 1, 1
    while a <= n do
      yield(a)
      a, b = b, a+b
    end
  end, n)
end

for i in generatefib(20) do print(i) end
