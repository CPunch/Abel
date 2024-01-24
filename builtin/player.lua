local export = {}

local function createAnim(plr, startTileID)
    local moving = plr:AddAnimation()
    local idle = plr:AddAnimation()

    -- 1 frame, lasting 250ms (we match the duration of the moving animation)
    plr:AddFrame(idle, startTileID, 250)

    -- 4 frames, each lasting 250ms
    plr:AddFrame(moving, startTileID + 1, 250)
    plr:AddFrame(moving, startTileID, 250)
    plr:AddFrame(moving, startTileID + 2, 250)
    plr:AddFrame(moving, startTileID, 250)
    return {
        moving = moving,
        idle = idle
    }
end

-- helper function to create a string key from a vector.
-- this is used to lookup animations in the table.
local function vecLookup(x, y)
    return tostring(Vec2.New(x, y))
end

function export.createPlayer(texture)
    local plr = Entity.New(texture, Vec2.New(32 * 6, 32 * 6))
    local selectedDir = Vec2.New(0, 0)
    local walkSpeed = Vec2.New(50, 50)
    local isSprinting = false

    Input.OnKeyUp(function(key)
        if (key == "W" or key == "Up") and selectedDir:Y() == -1 then
            selectedDir:SetY(0)
        elseif (key == "S" or key == "Down") and selectedDir:Y() == 1 then
            selectedDir:SetY(0)
        elseif (key == "A" or key == "Left") and selectedDir:X() == -1 then
            selectedDir:SetX(0)
        elseif (key == "D" or key == "Right") and selectedDir:X() == 1 then
            selectedDir:SetX(0)
        elseif (key == "Left Shift" or key == "Right Shift") and isSprinting then
            isSprinting = false
            walkSpeed:SetX(50)
            walkSpeed:SetY(50)
        end
    end)

    Input.OnKeyDown(function(key)
        if key == "W" or key == "Up" then
            selectedDir:SetY(-1)
        elseif key == "S" or key == "Down" then
            selectedDir:SetY(1)
        elseif key == "A" or key == "Left" then
            selectedDir:SetX(-1)
        elseif key == "D" or key == "Right" then
            selectedDir:SetX(1)
        elseif (key == "Left Shift" or key == "Right Shift") and not isSprinting then
            isSprinting = true
            walkSpeed:SetX(100)
            walkSpeed:SetY(100)
        end
    end)

    -- animations
    local animationLookupTable = {
        [vecLookup(0, 1)] = createAnim(plr, 0),
        [vecLookup(-1, 1)] = createAnim(plr, 3),
        [vecLookup(1, 1)] = createAnim(plr, 6),
        [vecLookup(-1, 0)] = createAnim(plr, 9),
        [vecLookup(1, 0)] = createAnim(plr, 12),
        [vecLookup(0, -1)] = createAnim(plr, 16),
        [vecLookup(1, -1)] = createAnim(plr, 19),
        [vecLookup(-1, -1)] = createAnim(plr, 22),
    }

    local idleAnim = plr:AddAnimation()
    plr:AddFrame(idleAnim, 0, 250)
    World.OnStep(function()
        -- update velocity
        local vel = selectedDir:Normalize() * walkSpeed
        plr:SetVelocity(vel)

        -- update animation
        local dir = vecLookup(selectedDir:X(), selectedDir:Y())
        local anims = animationLookupTable[dir]
        if anims then
            plr:PlayAnimation(anims.moving)
            idleAnim = anims.idle
        else
            plr:PlayAnimation(idleAnim)
        end
    end)

    World.SetFollow(plr)
    plr:Add()
    return plr
end

return export