-- co-rotina para gerar todos os números de 2 a n
function gen (n)
  return coroutine(function ()
    for i=2,n do yield(i) end
  end)
end

-- co-rotina que filtra os números gerados por 'g', tirando os múltiplos de 'p'
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
  local n = x()   -- pega um número
  if n == nil then break end    -- acabou?
  print(n)   -- número é primo
  x = filter(n, x)   -- tira fora seus múltiplos
end
