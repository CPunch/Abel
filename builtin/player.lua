local export = {}

local function createAnim(plr, startTileID)
    local moving = plr:AddAnimation()
    local idle = plr:AddAnimation()

    -- 2 frames
    plr:AddFrame(idle, startTileID, 1500)
    plr:AddFrame(idle, startTileID + 1, 100)

    -- 4 frames, each lasting 250ms
    plr:AddFrame(moving, startTileID + 2, 250)
    plr:AddFrame(moving, startTileID, 250)
    plr:AddFrame(moving, startTileID + 3, 250)
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

    -- animations
    local animationLookupTable = {
        [vecLookup(0, 1)] = createAnim(plr, 0),
        [vecLookup(-1, 1)] = createAnim(plr, 4),
        [vecLookup(1, 1)] = createAnim(plr, 8),
        [vecLookup(-1, 0)] = createAnim(plr, 12),
        [vecLookup(0, -1)] = createAnim(plr, 16),
        [vecLookup(1, -1)] = createAnim(plr, 20),
        [vecLookup(-1, -1)] = createAnim(plr, 24),
        [vecLookup(1, 0)] = createAnim(plr, 28),
    }

    -- you can set arbitrary fields on entities which can be accessed by any
    -- script with a reference to this entity.
    plr.selectedDir = Vec2.New(0, 0)
    plr.walkSpeed = Vec2.New(50, 50)
    plr.isSprinting = false

    -- or even override existing functions to add custom behavior.
    local oldAdd = plr.Add
    function plr:Add()
        plr.okuEvnt = Input.OnKeyUp(function(key)
            if (key == "W" or key == "Up") and plr.selectedDir:Y() == -1 then
                plr.selectedDir:SetY(0)
            elseif (key == "S" or key == "Down") and plr.selectedDir:Y() == 1 then
                plr.selectedDir:SetY(0)
            elseif (key == "A" or key == "Left") and plr.selectedDir:X() == -1 then
                plr.selectedDir:SetX(0)
            elseif (key == "D" or key == "Right") and plr.selectedDir:X() == 1 then
                plr.selectedDir:SetX(0)
            elseif (key == "Left Shift" or key == "Right Shift") and plr.isSprinting then
                plr.isSprinting = false
                plr.walkSpeed:SetX(50)
                plr.walkSpeed:SetY(50)
            end
        end)

        plr.okdEvnt = Input.OnKeyDown(function(key)
            if key == "W" or key == "Up" then
                plr.selectedDir:SetY(-1)
            elseif key == "S" or key == "Down" then
                plr.selectedDir:SetY(1)
            elseif key == "A" or key == "Left" then
                plr.selectedDir:SetX(-1)
            elseif key == "D" or key == "Right" then
                plr.selectedDir:SetX(1)
            elseif (key == "Left Shift" or key == "Right Shift") and not plr.isSprinting then
                plr.isSprinting = true
                plr.walkSpeed:SetX(100)
                plr.walkSpeed:SetY(100)
            end
        end)

        local idleAnim = animationLookupTable[vecLookup(0, 1)].idle
        plr.osEvnt = World.OnStep(function()
            -- update velocity
            local vel = plr.selectedDir:Normalize() * plr.walkSpeed
            plr:SetVelocity(vel)

            -- update animation
            local dir = vecLookup(plr.selectedDir:X(), plr.selectedDir:Y())
            local anims = animationLookupTable[dir]
            if anims then
                plr:PlayAnimation(anims.moving)
                idleAnim = anims.idle
            else
                plr:PlayAnimation(idleAnim)
            end
        end)

        oldAdd(self)
    end

    -- for example, we can override the Remove function to disconnect our
    -- event listeners.
    local oldRemove = plr.Remove
    function plr:Remove(vel)
        plr.okuEvnt:Disconnect()
        plr.okdEvnt:Disconnect()
        plr.osEvnt:Disconnect()
        World.SetFollow(nil)
        oldRemove(self, vel)
    end

    World.SetFollow(plr)
    return plr
end

return export