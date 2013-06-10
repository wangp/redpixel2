-- co-rotina para gerar todos os n�meros de 2 a n
function gen (n)
  return coroutine(function ()
    for i=2,n do yield(i) end
  end)
end

-- co-rotina que filtra os n�meros gerados por 'g', tirando os m�ltiplos de 'p'
function filter (p, g)
  return coroutine(function ()
    while 1 do
      local n = g()
      if n == nil then return end
      if mod(n, p) ~= 0 then yield(n) end
    end
  end)
end

x = gen(1000)   -- gerador inicial
while 1 do
  local n = x()   -- pega um n�mero
  if n == nil then break end    -- acabou?
  print(n)   -- n�mero � primo
  x = filter(n, x)   -- tira fora seus m�ltiplos
end
