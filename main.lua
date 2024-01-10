local vec = Vec2.New(0, 0)

while true do
  wait(1)

  vec = vec + Vec2.New(math.random(), math.random())
  print("Hello World! " .. tostring(vec:Normalize()) .. " : ".. vec:X() .. " " .. vec:Y())
end