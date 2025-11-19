-- scripts/player_fps.lua

Script = {
    position = Vec3.new(0.0, 1.7, 5.0),
    rotation = Vec3.new(0.0, 0.0, 0.0),  
    velocity = Vec3.new(0.0, 0.0, 0.0),
    
    -- Movement
    moveSpeed = 5.0,
    sprintSpeed = 10.0,
    jumpForce = 7.0,
    gravity = -20.0,
    
    -- Mouse
    mouseSensitivity = 0.1,
    lastMouseX = 0.0,
    lastMouseY = 0.0,
    firstMouse = true,
    invertY = true,  
	
    -- State
    isGrounded = true,
    isSprinting = false,
    
    cameraPosition = Vec3.new(0.0, 1.7, 5.0),
    cameraRotation = Vec3.new(0.0, 0.0, 0.0),
    isCamera = true  
}

function Script:Init(objectID)
    Log("FPS Player Controller initialized")
    self.objectID = objectID
    self.lastMouseX = 400.0 
    self.lastMouseY = 300.0 
end

function Script:Update(objectID, dt)
    self:UpdateMouseLook(dt)
    self:UpdateMovement(dt)
    
    -- Apply gravity
    if not self.isGrounded then
        self.velocity.y = self.velocity.y + (self.gravity * dt)
    end
    
    self.position.x = self.position.x + (self.velocity.x * dt)
    self.position.y = self.position.y + (self.velocity.y * dt)
    self.position.z = self.position.z + (self.velocity.z * dt)
    
    -- Ground collision (simple)
    if self.position.y < 1.7 then
        self.position.y = 1.7
        self.velocity.y = 0.0
        self.isGrounded = true
    else
        self.isGrounded = false
    end
    
    self.cameraPosition = self.position
    self.cameraRotation = self.rotation
    
    if self.rotation.x > 89.0 then
        self.rotation.x = 89.0
    elseif self.rotation.x < -89.0 then
        self.rotation.x = -89.0
    end
end

function Script:UpdateMouseLook(dt)
    local mouseX, mouseY = Input.GetMousePosition()
    
    if self.firstMouse then
        self.lastMouseX = mouseX
        self.lastMouseY = mouseY
        self.firstMouse = false
        return  
    end
    
    local xoffset = (mouseX - self.lastMouseX) * self.mouseSensitivity
    local yoffset = (mouseY - self.lastMouseY) * self.mouseSensitivity  

    self.lastMouseX = mouseX
    self.lastMouseY = mouseY

    self.rotation.y = self.rotation.y + xoffset  
    
    if self.invertY then
        self.rotation.x = self.rotation.x + yoffset  
    else
        self.rotation.x = self.rotation.x - yoffset  
    end
end

function Script:UpdateMovement(dt)
    local yawRad = Math.Radians(self.rotation.y)
    
    local forward = Vec3.new(
        Math.Sin(yawRad),
        0.0,
        -Math.Cos(yawRad)
    )
    
    local right = Vec3.new(
        Math.Cos(yawRad),
        0.0,
        Math.Sin(yawRad)
    )
    
    self.velocity.x = 0.0
    self.velocity.z = 0.0
    
    local speed = self.isSprinting and self.sprintSpeed or self.moveSpeed
    
    if Input.GetKeyDown("W") then  -- W
        self.velocity.x = self.velocity.x + (forward.x * speed)
        self.velocity.z = self.velocity.z + (forward.z * speed)
    end
    
    if Input.GetKeyDown("S") then  -- S
        self.velocity.x = self.velocity.x - (forward.x * speed)
        self.velocity.z = self.velocity.z - (forward.z * speed)
    end
    
    if Input.GetKeyDown("A") then  -- A
        self.velocity.x = self.velocity.x - (right.x * speed)
        self.velocity.z = self.velocity.z - (right.z * speed)
    end
    
    if Input.GetKeyDown("D") then  -- D
        self.velocity.x = self.velocity.x + (right.x * speed)
        self.velocity.z = self.velocity.z + (right.z * speed)
    end
    
    self.isSprinting = Input.GetKeyDown("LeftShift")
    
    if Input.GetKeyDown("Space") and self.isGrounded then
        self.velocity.y = self.jumpForce
        self.isGrounded = false
    end
end

function Script:GetForwardVector()
    local yawRad = Math.Radians(self.rotation.y)
    local pitchRad = Math.Radians(self.rotation.x)
    
    return Vec3.new(
        Math.Cos(pitchRad) * Math.Sin(yawRad),
        -Math.Sin(pitchRad),
        -Math.Cos(pitchRad) * Math.Cos(yawRad)
    )
end

function Script:GetRightVector()
    local yawRad = Math.Radians(self.rotation.y)
    return Vec3.new(
        Math.Cos(yawRad),
        0.0,
        Math.Sin(yawRad)
    )
end

function Script:GetUpVector()
    return Vec3.new(0.0, 1.0, 0.0)
end

function Script:FixedUpdate(objectID, fixedDt)
    -- Physics can go here
end

function Script:OnDestroy(objectID)
    Log("FPS Player Controller destroyed")
end