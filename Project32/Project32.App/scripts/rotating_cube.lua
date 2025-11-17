-- scripts/rotating_cube.lua

Script = {
    position = Vec3.new(0.0, 2.0, 0.0),
    size = Vec3.new(1.0, 1.0, 1.0),
    rotation = Vec3.new(0.0, 0.0, 0.0),
    rotationSpeed = 10.0, 
    color = Vec3.new(1.0, 0.5, 0.2),
    pulseSpeed = 0.1,
    time = 0.0
}

function Script:Init(objectID)
    Log("Rotating Cube initialized at center")
    self.objectID = objectID
end

function Script:Update(objectID, dt)
    self.time = self.time + dt
    
    self.rotation.y = self.rotation.y + (self.rotationSpeed * dt)
    if self.rotation.y > 360.0 then
        self.rotation.y = self.rotation.y - 360.0
    end
    
    self.rotation.x = self.rotation.x + (self.rotationSpeed * 0.5 * dt)
    if self.rotation.x > 360.0 then
        self.rotation.x = self.rotation.x - 360.0
    end
    
    local pulse = Math.Sin(self.time * self.pulseSpeed) * 0.2
    self.position.y = 1.5 + pulse
    
    local brightness = 0.5 + (Math.Sin(self.time * 3.0) * 0.25)
    self.color = Vec3.new(1.0, 0.5 * brightness, 0.2)
end

function Script:FixedUpdate(objectID, fixedDt)
end

function Script:OnDestroy(objectID)
    Log("Rotating Cube destroyed")
end