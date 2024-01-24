local plr = require("builtin/player")
local field = require("builtin/demoField")
local fieldTexture = Texture.Load("res/tileset.png", Vec2.New(16, 16))
local plrTexture = Texture.Load("res/entityset.png", Vec2.New(16, 16))

print(fieldTexture)
print(plrTexture)

field.load(fieldTexture)
plr.createPlayer(plrTexture)