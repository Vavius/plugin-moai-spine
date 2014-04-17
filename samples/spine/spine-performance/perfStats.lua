--------------------------------------------------------------------------------
-- Show performance stats on top of everything
--------------------------------------------------------------------------------

local layer
local fps
local drawCallsCount
local dragonCount

local function layoutStatsDisplay()
    fps:setLoc(-700, 1000)
    drawCallsCount:setLoc(-700, 980)
    dragonCount:setLoc(-700, 960)
end

local function Label(text, width, height, font, textSize)
    local label = MOAITextBox.new()

    label:setFont(font)
    label:setRect(0, 0, width or 10, height or 10)
    label:setTextSize(textSize or Font.DEFAULT_POINTS)
    label:setString(text)
    label:setYFlip(true)

    return label
end

local function createStatsDisplay(layer)
    local bitmapFont = MOAIFont.new ()
    bitmapFont:loadFromBMFont ( 'CopperPlateGothic.fnt' )
    fps = Label("60", 60, 18, bitmapFont, 16)
    fps:setShader(MOAIShaderMgr.getShader(MOAIShaderMgr.DECK2D_SHADER))
    layer:insertProp(fps)

    drawCallsCount = Label("60", 90, 18, bitmapFont, 16)
    drawCallsCount:setShader(MOAIShaderMgr.getShader(MOAIShaderMgr.DECK2D_SHADER))
    layer:insertProp(drawCallsCount)

    dragonCount = Label("0", 90, 18, bitmapFont, 16)
    dragonCount:setShader(MOAIShaderMgr.getShader(MOAIShaderMgr.DECK2D_SHADER))
    layer:insertProp(dragonCount)

    layoutStatsDisplay()

    perf_func = MOAICoroutine.new()
    perf_func:run(function()
        local deltaTime, prevElapsedTime = 0, 0
        while true do 
            for i = 1, 20 do 
                coroutine.yield()
            end
            fps:setString(tostring(MOAISim.getPerformance()))
            -- spf:setString(tostring(deltaTime))
            drawCallsCount:setString(tostring(MOAIRenderMgr.getPerformanceDrawCount()))
            dragonCount:setString(tostring(#dragons))
        end
    end)
end

return {new = createStatsDisplay}