-- example of for with generator functions

function generatefib (n)
  return coroutine.create(function (n)
    local a,b = 1, 1
    while a <= n do
      coroutine.yield(a)
      a, b = b, a+b
    end
  end, n)
end

for i in generatefib(50) do print(i) end
