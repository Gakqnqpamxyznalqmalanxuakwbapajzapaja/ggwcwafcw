local fov = 100
local smoothness = 10
local crosshairDistance = 5

local RunService = game:GetService("RunService")
local UserInputService = game:GetService("UserInputService")
local Players = game:GetService("Players")
local Cam = game.Workspace.CurrentCamera

local FOVring = Drawing.new("Circle")
FOVring.Visible = true
FOVring.Thickness = 2
FOVring.Color = Color3.fromRGB(0, 255, 0)
FOVring.Filled = false
FOVring.Radius = fov
FOVring.Position = Cam.ViewportSize / 2

local Player = Players.LocalPlayer
local PlayerGui = Player:WaitForChild("PlayerGui")

local ScreenGui = Instance.new("ScreenGui")
ScreenGui.Name = "FovAdjustGui"
ScreenGui.Parent = PlayerGui

local Frame = Instance.new("Frame")
Frame.Name = "MainFrame"
Frame.BackgroundColor3 = Color3.fromRGB(30, 30, 30)
Frame.BorderColor3 = Color3.fromRGB(128, 0, 128)
Frame.BorderSizePixel = 2
Frame.Position = UDim2.new(0.3, 0, 0.3, 0)
Frame.Size = UDim2.new(0.4, 0, 0.4, 0)
Frame.Active = true
Frame.Draggable = true
Frame.Parent = ScreenGui

local MinimizeButton = Instance.new("TextButton")
MinimizeButton.Name = "MinimizeButton"
MinimizeButton.Text = "-"
MinimizeButton.TextColor3 = Color3.fromRGB(255, 255, 255)
MinimizeButton.BackgroundColor3 = Color3.fromRGB(50, 50, 50)
MinimizeButton.Position = UDim2.new(0.9, 0, 0, 0)
MinimizeButton.Size = UDim2.new(0.1, 0, 0.1, 0)
MinimizeButton.Parent = Frame

local isMinimized = false

MinimizeButton.MouseButton1Click:Connect(function()
    isMinimized = not isMinimized
    if isMinimized then
        Frame:TweenSize(UDim2.new(0.1, 0, 0.1, 0), Enum.EasingDirection.Out, Enum.EasingStyle.Quad, 0.3, true)
        MinimizeButton.Text = "+"
    else
        Frame:TweenSize(UDim2.new(0.4, 0, 0.4, 0), Enum.EasingDirection.Out, Enum.EasingStyle.Quad, 0.3, true)
        MinimizeButton.Text = "-"
    end
end)

local FovLabel = Instance.new("TextLabel")
FovLabel.Name = "FovLabel"
FovLabel.Text = "自瞄范围"
FovLabel.TextColor3 = Color3.fromRGB(255, 255, 255)
FovLabel.BackgroundTransparency = 1
FovLabel.Position = UDim2.new(0.1, 0, 0.1, 0)
FovLabel.Size = UDim2.new(0.8, 0, 0.2, 0)
FovLabel.Parent = Frame

local FovSlider = Instance.new("TextBox")
FovSlider.Name = "FovSlider"
FovSlider.Text = tostring(fov)
FovSlider.TextColor3 = Color3.fromRGB(255, 255, 255)
FovSlider.BackgroundColor3 = Color3.fromRGB(50, 50, 50)
FovSlider.Position = UDim2.new(0.1, 0, 0.3, 0)
FovSlider.Size = UDim2.new(0.8, 0, 0.2, 0)
FovSlider.Parent = Frame

local SmoothnessLabel = Instance.new("TextLabel")
SmoothnessLabel.Name = "SmoothnessLabel"
SmoothnessLabel.Text = "自瞄平滑度"
SmoothnessLabel.TextColor3 = Color3.fromRGB(255, 255, 255)
SmoothnessLabel.BackgroundTransparency = 1
SmoothnessLabel.Position = UDim2.new(0.1, 0, 0.5, 0)
SmoothnessLabel.Size = UDim2.new(0.8, 0, 0.2, 0)
SmoothnessLabel.Parent = Frame

local SmoothnessSlider = Instance.new("TextBox")
SmoothnessSlider.Name = "SmoothnessSlider"
SmoothnessSlider.Text = tostring(smoothness)
SmoothnessSlider.TextColor3 = Color3.fromRGB(255, 255, 255)
SmoothnessSlider.BackgroundColor3 = Color3.fromRGB(50, 50, 50)
SmoothnessSlider.Position = UDim2.new(0.1, 0, 0.7, 0)
SmoothnessSlider.Size = UDim2.new(0.8, 0, 0.2, 0)
SmoothnessSlider.Parent = Frame

local CrosshairDistanceLabel = Instance.new("TextLabel")
CrosshairDistanceLabel.Name = "CrosshairDistanceLabel"
CrosshairDistanceLabel.Text = "自瞄预判距离"
CrosshairDistanceLabel.TextColor3 = Color3.fromRGB(255, 255, 255)
CrosshairDistanceLabel.BackgroundTransparency = 1
CrosshairDistanceLabel.Position = UDim2.new(0.1, 0, 0.9, 0)
CrosshairDistanceLabel.Size = UDim2.new(0.8, 0, 0.2, 0)
CrosshairDistanceLabel.Parent = Frame

local CrosshairDistanceSlider = Instance.new("TextBox")
CrosshairDistanceSlider.Name = "CrosshairDistanceSlider"
CrosshairDistanceSlider.Text = tostring(crosshairDistance)
CrosshairDistanceSlider.TextColor3 = Color3.fromRGB(255, 255, 255)
CrosshairDistanceSlider.BackgroundColor3 = Color3.fromRGB(50, 50, 50)
CrosshairDistanceSlider.Position = UDim2.new(0.1, 0, 1.1, 0)
CrosshairDistanceSlider.Size = UDim2.new(0.8, 0, 0.2, 0)
CrosshairDistanceSlider.Parent = Frame

local targetCFrame = Cam.CFrame

local function updateDrawings()
    local camViewportSize = Cam.ViewportSize
    FOVring.Position = camViewportSize / 2
    FOVring.Radius = fov
end

local function onKeyDown(input)
    if input.KeyCode == Enum.KeyCode.Delete then
        RunService:UnbindFromRenderStep("FOVUpdate")
        FOVring:Remove()
    end
end

UserInputService.InputBegan:Connect(onKeyDown)

local function getClosestPlayerInFOV(trg_part)
    local nearest = nil
    local last = math.huge
    local playerMousePos = Cam.ViewportSize / 2

    for _, player in ipairs(Players:GetPlayers()) do
        if player ~= Players.LocalPlayer then
            local part = player.Character and player.Character:FindFirstChild(trg_part)
            if part then
                local ePos, isVisible = Cam:WorldToViewportPoint(part.Position)
                local distance = (Vector2.new(ePos.x, ePos.y) - playerMousePos).Magnitude

                if distance < last and isVisible and distance < fov then
                    last = distance
                    nearest = player
                end
            end
        end
    end

    return nearest
end

RunService.RenderStepped:Connect(function()
    updateDrawings()

    local closest = getClosestPlayerInFOV("Head")
    if closest and closest.Character:FindFirstChild("Head") then
        local targetCharacter = closest.Character
        local targetHead = targetCharacter.Head
        local targetRootPart = targetCharacter:FindFirstChild("HumanoidRootPart")

        local isMoving = targetRootPart.Velocity.Magnitude > 0.1

        local targetPosition
        if isMoving then
            targetPosition = targetHead.Position + (targetHead.CFrame.LookVector * crosshairDistance)
        else
            targetPosition = targetHead.Position
        end

        targetCFrame = CFrame.new(Cam.CFrame.Position, targetPosition)
    else
        targetCFrame = Cam.CFrame
    end
    Cam.CFrame = Cam.CFrame:Lerp(targetCFrame, 1 / smoothness)
end)

FovSlider.FocusLost:Connect(function(enterPressed, inputThatCausedFocusLoss)
    if enterPressed then
        local newFov = tonumber(FovSlider.Text)
        if newFov then
            fov = newFov
        else
            FovSlider.Text = tostring(fov)
        end
    end
end)

SmoothnessSlider.FocusLost:Connect(function(enterPressed, inputThatCausedFocusLoss)
    if enterPressed then
        local newSmoothness = tonumber(SmoothnessSlider.Text)
        if newSmoothness then
            smoothness = newSmoothness
        else
            SmoothnessSlider.Text = tostring(smoothness)
        end
    end
end)

CrosshairDistanceSlider.FocusLost:Connect(function(enterPressed, inputThatCausedFocusLoss)
    if enterPressed then
        local newCrosshairDistance = tonumber(CrosshairDistanceSlider.Text)
        if newCrosshairDistance then
            crosshairDistance = newCrosshairDistance
        else
            CrosshairDistanceSlider.Text = tostring(crosshairDistance)
        end
    end
end)