----------------------------------------------------------------
-- Copyright (c) 2010-2011 Zipline Games, Inc. 
-- All Rights Reserved. 
-- http://getmoai.com
----------------------------------------------------------------

MOAISim.openWindow ( "test", 480, 640 )

MOAISim.clearLoopFlags()

MOAISim.setLoopFlags(MOAISim.SIM_LOOP_ALLOW_BOOST)
MOAISim.setLoopFlags(MOAISim.SIM_LOOP_LONG_DELAY)
MOAISim.setBoostThreshold(0)

viewport = MOAIViewport.new ()
viewport:setSize ( 480, 640 )
viewport:setScale ( 480, -640 )

layer = MOAILayer2D.new ()
layer:setViewport ( viewport )
MOAISim.pushRenderPass ( layer )

local goblinsData = MOAISpineSkeletonData.new ()
goblinsData:load("goblins/goblins-ffd.json", "goblins/goblins-ffd.atlas", 1)

local goblins = MOAISpineSkeleton.new ()
goblins:init(goblinsData)
goblins:setFlip(false, true)
goblins:setToSetupPose()
goblins:setSkin("goblin")
goblins:initAnimationState()
goblins:setAnimation (1, "walk", true)

-- By default MOAISpineSkeleton uses cached bounds for better performance. 
-- Bounds are recomputed: 
-- 1) when called setToSetupPose, setBonesToSetupPose, setSlotsToSetupPose or setSkin
-- 2) if setComputeBounds is true, bounds will be updated every frame
goblins:setComputeBounds (true)

goblins:start()
goblins:setLoc(0, 100, 0)

goblins:moveRot(0, 0, 360, 4)

layer:insertProp(goblins)

goblins:setAttachment("right hand item 2", "shield")


--============================================================================--
-- Debug draw (spear and shild attachments)
--============================================================================--
function onDraw ( index, xOff, yOff, xFlip, yFlip )

	local spear = goblins:getAttachmentVertices("left hand item", "spear")
	local shield = goblins:getAttachmentVertices("right hand item 2", "shield")

	-- add initial vertices to close the region
	table.insert(spear, spear[1])
	table.insert(spear, spear[2])

	table.insert(shield, shield[1])
	table.insert(shield, shield[2])

	MOAIGfxDevice.setPenColor ( 1, 0, 0, 1 )
	MOAIDraw.drawLine(spear)

	MOAIGfxDevice.setPenColor ( 1, 1, 0, 1 )
	MOAIDraw.drawLine(shield)
end

scriptDeck = MOAIScriptDeck.new ()
scriptDeck:setRect ( -200, -200, 200, 200 )
scriptDeck:setDrawCallback ( onDraw )

prop = MOAIProp2D.new ()
prop:setDeck ( scriptDeck )
layer:insertProp ( prop )

prop:setParent(goblins)

