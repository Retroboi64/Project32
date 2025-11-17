-- scripts/floor.lua

Script = {
    position = Vec3.new(0.0, 0.0, 0.0),
    size = Vec3.new(20.0, 0.1, 20.0),
    color = Vec3.new(0.3, 0.3, 0.3),
    isStatic = true
}

function Script:Init(objectID)
    Log("Floor initialized: " .. self.size.x .. "x" .. self.size.z)
    self.objectID = objectID
end

function Script:Update(objectID, dt)
end

function Script:FixedUpdate(objectID, fixedDt)
end

function Script:OnDestroy(objectID)
    Log("Floor destroyed")
end