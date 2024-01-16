local vec = Vec2.New(0, 0)

function step()
  vec = vec + Vec2.New(math.random(), math.random())
  print("Hello World! " .. tostring(vec:Normalize()) .. " : ".. vec:X() .. " " .. vec:Y())

  -- return a random number of milliseconds
  -- to wait before calling this function again
  return math.random(1000)
end

-- starts 5 tasks
for i = 1, 5 do 
  -- schedule this function to be (almost) immediately called
  startTask(step, 1)
end