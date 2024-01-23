local export = {}

local function createAnim(plr, startTileID)
    local id = plr:AddAnimation()
    plr:AddFrame(id, startTileID + 1, 250)
    plr:AddFrame(id, startTileID, 250)
    plr:AddFrame(id, startTileID + 2, 250)
    plr:AddFrame(id, startTileID, 250)
    return id
end

function export:createPlayer()
    local text = Texture.Load("res/entityset.png", Vec2.New(16, 16))
    local plr = Entity.New(text, Vec2.New(32 * 6, 32 * 6))
    local selectedDir = Vec2.New(0, 0)
    local walkSpeed = Vec2.New(64, 64)

    Input.OnKeyUp(function(key)
        if key == "W" or key == "Up" and selectedDir:Y() == -1 then
            selectedDir = Vec2.New(selectedDir:X(), 0)
        elseif key == "S" or key == "Down" and selectedDir:Y() == 1 then
            selectedDir = Vec2.New(selectedDir:X(), 0)
        elseif key == "A" or key == "Left" and selectedDir:X() == -1 then
            selectedDir = Vec2.New(0, selectedDir:Y())
        elseif key == "D" or key == "Right" and selectedDir:X() == 1 then
            selectedDir = Vec2.New(0, selectedDir:Y())
        end
    end)

    Input.OnKeyDown(function(key)
        if key == "W" or key == "Up" then
            selectedDir = Vec2.New(selectedDir:X(), -1)
        elseif key == "S" or key == "Down" then
            selectedDir = Vec2.New(selectedDir:X(), 1)
        elseif key == "A" or key == "Left" then
            selectedDir = Vec2.New(-1, selectedDir:Y())
        elseif key == "D" or key == "Right" then
            selectedDir = Vec2.New(1, selectedDir:Y())
        end
    end)

    -- animations
    local downAnim = createAnim(plr, 0)
    local downLeftAnim = createAnim(plr, 3)
    local downRightAnim = createAnim(plr, 6)
    local leftAnim = createAnim(plr, 9)
    local rightAnim = createAnim(plr, 12)
    local upAnim = createAnim(plr, 16)
    local upRightAnim = createAnim(plr, 19)
    local upLeftAnim = createAnim(plr, 22)
    local idleAnim = plr:AddAnimation()
    plr:AddFrame(idleAnim, 0, 100)

    plr:Add() -- adds to world
    World.SetFollow(plr)
    World.OnStep(function()
        local vel = selectedDir:Normalize() * walkSpeed
        plr:SetVelocity(vel)

        -- update animation
        if selectedDir:X() == 0 and selectedDir:Y() == 1 then
            plr:PlayAnimation(downAnim)
        elseif selectedDir:X() == 0 and selectedDir:Y() == -1 then
            plr:PlayAnimation(upAnim)
        elseif selectedDir:X() == 1 and selectedDir:Y() == 1 then
            plr:PlayAnimation(downRightAnim)
        elseif selectedDir:X() == -1 and selectedDir:Y() == 1 then
            plr:PlayAnimation(downLeftAnim)
        elseif selectedDir:X() == 1 and selectedDir:Y() == -1 then
            plr:PlayAnimation(upRightAnim)
        elseif selectedDir:X() == -1 and selectedDir:Y() == -1 then
            plr:PlayAnimation(upLeftAnim)
        elseif selectedDir:X() == 1 and selectedDir:Y() == 0 then
            plr:PlayAnimation(rightAnim)
        elseif selectedDir:X() == -1 and selectedDir:Y() == 0 then
            plr:PlayAnimation(leftAnim)
        else
            plr:PlayAnimation(idleAnim)
        end
    end)

    return plr
end

return export