----------------------------------------------------------------
-- Copyright (c) 2010-2011 Zipline Games, Inc. 
-- All Rights Reserved. 
-- http://getmoai.com
----------------------------------------------------------------

local perfStats = require("perfStats")

MOAISim.openWindow ( "test", 1536, 2048 )

MOAISim.clearLoopFlags()

MOAISim.setLoopFlags(MOAISim.SIM_LOOP_ALLOW_BOOST)
MOAISim.setLoopFlags(MOAISim.SIM_LOOP_LONG_DELAY)
MOAISim.setBoostThreshold(0)

local skeletonDataCache = setmetatable({}, {__mode = "v"})
function GetSkeletonData(dataPath, atlasPath, scale)
    local key = dataPath .. "$" .. atlasPath .. "$" .. scale
    local data = skeletonDataCache[key]

    if not data then
        data = MOAISpineSkeletonData.new()
        data:load(dataPath, atlasPath, scale)
        skeletonDataCache[key] = data        
    end
    return data
end

viewport = MOAIViewport.new ()
viewport:setSize ( 1536, 2048 )
viewport:setScale ( 1536, 2048 )

layer = MOAILayer2D.new ()
layer:setViewport ( viewport )
MOAISim.pushRenderPass ( layer )

perfStats.new(layer)

math.random() math.random() math.random()

function makeDragon(layer)
    local w, h = 1000, 1500

    local dragon = MOAISpineSkeleton.new ()
    dragon:init(GetSkeletonData("dragon/dragon.json", "dragon/dragon.atlas", 1 ))
    dragon:setToSetupPose()
    dragon:initAnimationState()
    dragon:setAnimation (1, "flying", true)

    dragon:start()

    local x, y = -0.5 * w + w * math.random(), -0.5 * h + h * math.random()
    dragon:setLoc(x, y, 0)

    layer:insertProp(dragon)

    return dragon
end

dragons = {}

MOAISim.setHistogramEnabled(true)

local grow = true
local maxDragons = 100

local input = MOAIInputMgr.device.mouseLeft or MOAIInputMgr.device.touch
input:setCallback(function()
    if grow and #dragons > maxDragons then
        grow = false
    elseif not grow and #dragons < 1 then
        grow = true
    end

    if grow then
        dragons[#dragons + 1] = makeDragon(layer)
    else
        layer:removeProp(dragons[#dragons])
        dragons[#dragons]:stop()
        dragons[#dragons] = nil
    end

    MOAISim.forceGarbageCollection()
    MOAISim.reportHistogram()
end)


