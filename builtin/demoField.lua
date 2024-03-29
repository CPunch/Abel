local export = {}
local MapSize = 64

local function createRandomCell(x, y)
    local rand = math.random(0, 6)
    
    if rand == 0 then
        World.SetCell(Vec2.New(x, y), 0, nil, false)
    elseif rand == 1 then
        World.SetCell(Vec2.New(x, y), 2, nil, false)
    else 
        World.SetCell(Vec2.New(x, y), 1, nil, false)
    end
end

function export.load(tileSet)
    print(tileSet)
    World.SetTileSet(tileSet)

    for i = 0, MapSize-1 do
        for j = 0, MapSize-1 do
            createRandomCell(i, j)
        end
    end

    World.SetCell(Vec2.New(3, 4), 3, nil, true)
    World.SetCell(Vec2.New(3, 5), 3, nil, true)
    World.SetCell(Vec2.New(3, 6), 3, nil, true)
    World.SetCell(Vec2.New(4, 6), 3, nil, true)
    World.SetCell(Vec2.New(5, 6), 3, nil, true)
end

return export