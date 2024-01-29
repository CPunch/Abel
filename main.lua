local plr = require("builtin/player")
local field = require("builtin/demoField")
local fieldTexture = Texture.Load("res/tileset.png", Vec2.New(16, 16))
local plrTexture = Texture.Load("res/entityset.png", Vec2.New(16, 16))

-- load our demo field
field.load(fieldTexture)

local player = plr.createPlayer(plrTexture)
player:Add()

-- access the walkSpeed field we set in builtin/player.lua
print(player.walkSpeed)
