--// Cache

local game, workspace = game, workspace
local getrawmetatable, getmetatable, setmetatable, pcall, getgenv, next, tick, select = getrawmetatable, getmetatable, setmetatable, pcall, getgenv, next, tick, select
local Vector2new, Vector3new, Vector3zero, CFramenew, Color3fromRGB, Color3fromHSV, Drawingnew, TweenInfonew = Vector2.new, Vector3.new, Vector3.zero, CFrame.new, Color3.fromRGB, Color3.fromHSV, Drawing.new, TweenInfo.new
local getupvalue, mousemoverel, tablefind, tableremove, stringlower, stringsub, mathclamp = debug.getupvalue, mousemoverel or (Input and Input.MouseMove), table.find, table.remove, string.lower, string.sub, math.clamp

local GameMetatable = getrawmetatable(game)
local __index = GameMetatable.__index
local __newindex = GameMetatable.__newindex
local GetService = __index(game, "GetService")

--// Services

local RunService = GetService(game, "RunService")
local UserInputService = GetService(game, "UserInputService")
local TweenService = GetService(game, "TweenService")
local Players = GetService(game, "Players")

--// Degrade "__index" and "__newindex" functions if the executor doesn't support "getrawmetatable" properly.

local ReciprocalRelativeSensitivity = false

if select(2, pcall(__index, Players, "LocalPlayer")) then
    ReciprocalRelativeSensitivity = true

    __index, __newindex = function(Object, Key)
        return Object[Key]
    end, function(Object, Key, Value)
        Object[Key] = Value
    end
end

--// Service Methods

local LocalPlayer = __index(Players, "LocalPlayer")
local Camera = __index(workspace, "CurrentCamera")

local FindFirstChild, FindFirstChildOfClass = __index(game, "FindFirstChild"), __index(game, "FindFirstChildOfClass")
local GetDescendants = __index(game, "GetDescendants")
local WorldToViewportPoint = __index(Camera, "WorldToViewportPoint")
local GetPartsObscuringTarget = __index(Camera, "GetPartsObscuringTarget")
local GetMouseLocation = __index(UserInputService, "GetMouseLocation")
local GetPlayers = __index(Players, "GetPlayers")

--// Variables

local RequiredDistance, Typing, Running, ServiceConnections, Animation, OriginalSensitivity = 2000, false, false, {}
local Connect, Disconnect, GetRenderProperty, SetRenderProperty = __index(game, "DescendantAdded").Connect

local UWP = false

do
    xpcall(function()
        local TemporaryDrawing = Drawingnew("Line")
        GetRenderProperty = getupvalue(getmetatable(TemporaryDrawing).__index, 4)
        SetRenderProperty = getupvalue(getmetatable(TemporaryDrawing).__newindex, 4)
        TemporaryDrawing.Remove(TemporaryDrawing)
    end, function()
        UWP, GetRenderProperty, SetRenderProperty = true, function(Object, Key)
            return Object[Key]
        end, function(Object, Key, Value)
            Object[Key] = Value
        end
    end)

    local TemporaryConnection = Connect(__index(game, "DescendantAdded"), function() end)
    Disconnect = TemporaryConnection.Disconnect
    Disconnect(TemporaryConnection)
end

--// 检查是否有多个进程

if ExunysDeveloperAimbot then
    ExunysDeveloperAimbot:Exit()
end

--// 环境变量

getgenv().ExunysDeveloperAimbot = {
    DeveloperSettings = {
        UpdateMode = "RenderStepped",
        TeamCheckOption = "TeamColor",
        RainbowSpeed = 1 -- Bigger = Slower
    },

    Settings = {
        Enabled = true,

        TeamCheck = false,
        AliveCheck = true,
        WallCheck = false,

        OffsetToMoveDirection = false,
        OffsetIncrement = 15,

        Sensitivity = 0, -- Animation length (in seconds) before fully locking onto target
        Sensitivity2 = 3.5, -- mousemoverel Sensitivity

        LockMode = 1, -- 1 = CFrame; 2 = mousemoverel
        LockPart = "Head", -- Body part to lock on

        TriggerKey = Enum.UserInputType.MouseButton2,
        Toggle = false
    },

    FOVSettings = {
        Enabled = true,
        Visible = true,

        Radius = 90,
        NumSides = 60,

        Thickness = 1,
        Transparency = 1,
        Filled = false,

        RainbowColor = false,
        RainbowOutlineColor = false,
        Color = Color3fromRGB(255, 255, 255),
        OutlineColor = Color3fromRGB(0, 0, 0),
        LockedColor = Color3fromRGB(255, 150, 150)
    },

    Blacklisted = {},
    FOVCircle = Drawingnew("Circle"),
    FOVCircleOutline = Drawingnew("Circle")
}

local Environment = getgenv().ExunysDeveloperAimbot

SetRenderProperty(Environment.FOVCircle, "Visible", false)
SetRenderProperty(Environment.FOVCircleOutline, "Visible", false)

--// 核心函数

local FixUsername = function(String)
    local Result

    for _, Value in next, GetPlayers(Players) do
        local Name = __index(Value, "Name")

        if stringsub(stringlower(Name), 1, #String) == stringlower(String) then
            Result = Name
        end
    end

    return Result
end

local GetRainbowColor = function()
    local RainbowSpeed = Environment.DeveloperSettings.RainbowSpeed

    return Color3fromHSV(tick() % RainbowSpeed / RainbowSpeed, 1, 1)
end

local ConvertVector = function(Vector)
    return Vector2new(Vector.X, Vector.Y)
end

local CancelLock = function()
    Environment.Locked = nil

    local FOVCircle = UWP and Environment.FOVCircle or Environment.FOVCircle.__OBJECT

    SetRenderProperty(FOVCircle, "Color", Environment.FOVSettings.Color)
    __newindex(UserInputService, "MouseDeltaSensitivity", OriginalSensitivity)

    if Animation then
        Animation:Cancel()
    end
end

local GetClosestPlayer = function()
    local Settings = Environment.Settings
    local LockPart = Settings.LockPart

    if not Environment.Locked then
        RequiredDistance = Environment.FOVSettings.Enabled and Environment.FOVSettings.Radius or 2000

        for _, Value in next, GetPlayers(Players) do
            local Character = __index(Value, "Character")
            local Humanoid = Character and FindFirstChildOfClass(Character, "Humanoid")

            if Value ~= LocalPlayer and not tablefind(Environment.Blacklisted, __index(Value, "Name")) and Character and FindFirstChild(Character, LockPart) and Humanoid then
                local PartPosition, TeamCheckOption = __index(Character[LockPart], "Position"), Environment.DeveloperSettings.TeamCheckOption

                if Settings.TeamCheck and __index(Value, TeamCheckOption) == __index(LocalPlayer, TeamCheckOption) then
                    continue
                end

                if Settings.AliveCheck and __index(Humanoid, "Health") <= 0 then
                    continue
                end

                if Settings.WallCheck then
                    local BlacklistTable = GetDescendants(__index(LocalPlayer, "Character"))

                    for _, Value in next, GetDescendants(Character) do
                        BlacklistTable[#BlacklistTable + 1] = Value
                    end

                    if #GetPartsObscuringTarget(Camera, {PartPosition}, BlacklistTable) > 0 then
                        continue
                    end
                end

                local Vector, OnScreen, Distance = WorldToViewportPoint(Camera, PartPosition)
                Vector = ConvertVector(Vector)
                Distance = (GetMouseLocation(UserInputService) - Vector).Magnitude

                if Distance < RequiredDistance and OnScreen then
                    RequiredDistance, Environment.Locked = Distance, Value
                end
            end
        end
    elseif (GetMouseLocation(UserInputService) - ConvertVector(WorldToViewportPoint(Camera, __index(__index(__index(Environment.Locked, "Character"), LockPart), "Position")))).Magnitude > RequiredDistance then
        CancelLock()
    end
end

local Load = function()
    OriginalSensitivity = __index(UserInputService, "MouseDeltaSensitivity")

    local Settings, FOVCircle, FOVCircleOutline, FOVSettings, Offset = Environment.Settings, Environment.FOVCircle, Environment.FOVCircleOutline, Environment.FOVSettings
    local OffsetToMoveDirection, LockPart = Settings.OffsetToMoveDirection, Settings.LockPart


    if FOVSettings.Enabled then
        SetRenderProperty(FOVCircle, "Visible", FOVSettings.Visible)
        SetRenderProperty(FOVCircleOutline, "Visible", FOVSettings.Visible)

        SetRenderProperty(FOVCircle, "Radius", FOVSettings.Radius)
        SetRenderProperty(FOVCircleOutline, "Radius", FOVSettings.Radius + 1)

        SetRenderProperty(FOVCircle, "NumSides", FOVSettings.NumSides)
        SetRenderProperty(FOVCircleOutline, "NumSides", FOVSettings.NumSides)

        SetRenderProperty(FOVCircle, "Thickness", FOVSettings.Thickness)
        SetRenderProperty(FOVCircleOutline, "Thickness", FOVSettings.Thickness + 1)

        SetRenderProperty(FOVCircle, "Transparency", FOVSettings.Transparency)
        SetRenderProperty(FOVCircleOutline, "Transparency", FOVSettings.Transparency)

        SetRenderProperty(FOVCircle, "Filled", FOVSettings.Filled)

        if FOVSettings.RainbowColor then
            SetRenderProperty(FOVCircle, "Color", GetRainbowColor())
        else
            SetRenderProperty(FOVCircle, "Color", FOVSettings.Color)
        end

        if FOVSettings.RainbowOutlineColor then
            SetRenderProperty(FOVCircleOutline, "Color", GetRainbowColor())
        else
            SetRenderProperty(FOVCircleOutline, "Color", FOVSettings.OutlineColor)
        end

        local MouseLocation = GetMouseLocation(UserInputService)
        SetRenderProperty(FOVCircle, "Position", MouseLocation)
        SetRenderProperty(FOVCircleOutline, "Position", MouseLocation)
    end

    local AimbotStepConnection = RunService[Environment.DeveloperSettings.UpdateMode]:Connect(function()
        if Environment.Settings.Enabled then
            GetClosestPlayer()

            if Environment.Locked then
                local LockedCharacter = __index(Environment.Locked, "Character")
                local LockedHumanoid = FindFirstChildOfClass(LockedCharacter, "Humanoid")
                local LockedPosition = __index(LockedCharacter[LockPart], "Position")

                if not LockedCharacter or not LockedHumanoid or __index(LockedHumanoid, "Health") <= 0 then
                    CancelLock()
                else
                    if Settings.LockMode == 1 then
                        if OffsetToMoveDirection then
                            LockedPosition += __index(__index(LockedCharacter, "HumanoidRootPart"), "CFrame").LookVector * Settings.OffsetIncrement
                        end

                        local AnimationTime = Settings.Sensitivity
                        local AnimationGoal = {CFrame = CFramenew(__index(Camera, "CFrame").Position, LockedPosition)}
                        local AnimationInformation = TweenInfonew(AnimationTime, Enum.EasingStyle.Sine, Enum.EasingDirection.Out)

                        if Animation then
                            Animation:Cancel()
                        end

                        Animation = TweenService:Create(Camera, AnimationInformation, AnimationGoal)
                        Animation:Play()
                    elseif Settings.LockMode == 2 then
                        if OffsetToMoveDirection then
                            LockedPosition += __index(__index(LockedCharacter, "HumanoidRootPart"), "CFrame").LookVector * Settings.OffsetIncrement
                        end

                        local MouseLocation = GetMouseLocation(UserInputService)
                        local LockedVector = ConvertVector(WorldToViewportPoint(Camera, LockedPosition))
                        local MousePosition = MouseLocation - LockedVector

                        mousemoverel(MousePosition.X / Settings.Sensitivity2, MousePosition.Y / Settings.Sensitivity2)
                    end

                    local FOVCircle = UWP and Environment.FOVCircle or Environment.FOVCircle.__OBJECT
                    SetRenderProperty(FOVCircle, "Color", FOVSettings.LockedColor)
                    __newindex(UserInputService, "MouseDeltaSensitivity", OriginalSensitivity / 2)
                end
            end
        end
    end)

    table.insert(ServiceConnections, AimbotStepConnection)
end

local Unload = function()
    Environment.Settings.Enabled = false

    CancelLock()

    if not UWP then
        Environment.FOVCircle.__OBJECT, Environment.FOVCircleOutline.__OBJECT = Environment.FOVCircle:Remove(), Environment.FOVCircleOutline:Remove()
    else
        Environment.FOVCircle:Remove()
        Environment.FOVCircleOutline:Remove()
    end

    for _, Connection in next, ServiceConnections do
        Connection:Disconnect()
    end

    ExunysDeveloperAimbot = nil
end

Environment.Load = Load
Environment.Unload = Unload

Load()
return LOL