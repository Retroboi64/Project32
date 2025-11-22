-- scripts/game_hud.lua

Script = {
    windowID = 0,
    hudWindow = -1,
    fpsText = -1,
    positionText = -1,
    velocityText = -1,
    healthBar = -1,
    staminaBar = -1,
    
    health = 100.0,
    maxHealth = 100.0,
    stamina = 100.0,
    maxStamina = 100.0,
    
    frameCount = 0,
    fpsTimer = 0.0,
    currentFPS = 0
}

function Script:Init(objectID)
    self.objectID = objectID
    self.windowID = Window.GetMainWindowID()
    
    Log("Game HUD initialized")
    
    self:CreateHUD()
end

function Script:CreateHUD()
    self.hudWindow = UI.CreateWindow("HUD", self.windowID)
    
    self.fpsText = UI.CreateText(self.windowID, "FPS: 0")
    UI.AddChild(self.hudWindow, self.fpsText)
    
    self.positionText = UI.CreateText(self.windowID, "Position: (0, 0, 0)")
    UI.AddChild(self.hudWindow, self.positionText)
    
    self.velocityText = UI.CreateText(self.windowID, "Velocity: (0, 0, 0)")
    UI.AddChild(self.hudWindow, self.velocityText)
    
    UI.CreateSeparator(self.windowID)
    
    local healthLabel = UI.CreateText(self.windowID, "Health")
    UI.AddChild(self.hudWindow, healthLabel)
    
    self.healthBar = UI.CreateSlider(self.windowID, "##health", 0.0, self.maxHealth, self.health)
    UI.AddChild(self.hudWindow, self.healthBar)
    
    local staminaLabel = UI.CreateText(self.windowID, "Stamina")
    UI.AddChild(self.hudWindow, staminaLabel)
    
    self.staminaBar = UI.CreateSlider(self.windowID, "##stamina", 0.0, self.maxStamina, self.stamina)
    UI.AddChild(self.hudWindow, self.staminaBar)
    
    UI.SetPosition(self.hudWindow, 10, 10)
    UI.SetSize(self.hudWindow, 300, 250)
    
    Log("HUD created successfully")
end

function Script:Update(objectID, dt)
    self.fpsTimer = self.fpsTimer + dt
    self.frameCount = self.frameCount + 1
    
    if self.fpsTimer >= 0.5 then
        self.currentFPS = math.floor(self.frameCount / self.fpsTimer)
        self.frameCount = 0
        self.fpsTimer = 0.0
    end
    
    UI.SetText(self.fpsText, "FPS: " .. self.currentFPS)
    
    local player = GetScript(0)
    if player then
        if player.position then
            local pos = player.position
            UI.SetText(self.positionText, string.format("Position: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z))
        end
        
        if player.velocity then
            local vel = player.velocity
            UI.SetText(self.velocityText, string.format("Velocity: (%.2f, %.2f, %.2f)", vel.x, vel.y, vel.z))
        end
        
        if player.isGrounded ~= nil then
            local groundStatus = player.isGrounded and "Grounded" or "Airborne"
            UI.SetText(self.velocityText, UI.GetText(self.velocityText) .. " | " .. groundStatus)
        end
    end
    
    self:UpdateHealthStamina(dt)
    
    UI.SetValue(self.healthBar, self.health)
    UI.SetValue(self.staminaBar, self.stamina)
    
    if Input.GetKeyDown("H") then
        UI.SetVisible(self.hudWindow, not UI.IsVisible(self.hudWindow))
    end
end

function Script:UpdateHealthStamina(dt)
    local player = GetScript(0)
    if not player then return end
    
    if player.isSprinting and player.isGrounded then
        self.stamina = Math.Max(0.0, self.stamina - (20.0 * dt))
    else
        self.stamina = Math.Min(self.maxStamina, self.stamina + (15.0 * dt))
    end
    
    if self.health < self.maxHealth then
        self.health = Math.Min(self.maxHealth, self.health + (2.0 * dt))
    end
end

function Script:TakeDamage(amount)
    self.health = Math.Max(0.0, self.health - amount)
    Log("Health: " .. self.health .. "/" .. self.maxHealth)
    
    if self.health <= 0.0 then
        Log("Player died!")
    end
end

function Script:Heal(amount)
    self.health = Math.Min(self.maxHealth, self.health + amount)
    Log("Healed! Health: " .. self.health .. "/" .. self.maxHealth)
end

function Script:FixedUpdate(objectID, fixedDt)
end

function Script:OnDestroy(objectID)
    if self.hudWindow ~= -1 then
        UI.Remove(self.hudWindow)
    end
    Log("Game HUD destroyed")
end