print("Welcome to Lua test")

bmp = create_bitmap(20,20)
clear(bmp)
line(bmp, 0, 0, 19, 19, makecol(255,0,0))
line(bmp, 19, 0, 0, 19, makecol(0,255,0))

data = load_datafile("/home/tjaden/red2/data/rpg.dat")

player = { x=0, y=0 }

while key(KEY_Q) == 0 do
  
  draw_sprite(screen, BITMAP(dat(data, 0)), 0, 0)
  draw_sprite(screen, bmp, player.x, player.y)
  
  if (key(KEY_D) ~= 0) then player.x = player.x + 1 end
  if (key(KEY_A) ~= 0) then player.x = player.x - 1 end
  if (key(KEY_W) ~= 0) then player.y = player.y - 1 end
  if (key(KEY_S) ~= 0) then player.y = player.y + 1 end
  
  rest(10)
  
end

destroy_bitmap(bmp)
unload_datafile(data)
