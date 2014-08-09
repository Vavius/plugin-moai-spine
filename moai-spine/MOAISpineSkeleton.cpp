// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <float.h>
#include <moai-spine/MOAISpineSkeleton.h>
#include <moai-spine/MOAISpineBone.h>
#include <moai-spine/MOAISpineSkeletonData.h>
#include <moai-spine/MOAISpineSlot.h>

//================================================================//
// Spine event listener
//================================================================//
static void callback (spAnimationState* state, int trackIndex, spEventType type, spEvent* event, int loopCount) {
	((MOAISpineSkeleton*) state->rendererObject )->OnAnimationEvent ( trackIndex, type, event, loopCount );
}

//================================================================//
// lua
//================================================================//


//----------------------------------------------------------------//
/**	@name	addAnimation
 
	@in		MOAISpineSkeleton self
	@in		number	track Id
	@in		string	animation name
	@in		bool	loop
	@in		number	delay
	@out	nil
 */
int MOAISpineSkeleton::_addAnimation ( lua_State *L ) {
	MOAI_LUA_SETUP( MOAISpineSkeleton, "UNSBN" )
	
	int trackId = state.GetValue < int >( 2, 0 );
	cc8* name = state.GetValue < cc8* >( 3, "");
	bool loop = state.GetValue < bool >( 4, false );
	float delay = state.GetValue < float >( 5, 0.f );
	
	if ( !self->mSkeleton || !self->mAnimationState ) {
		MOAIPrint ( "MOAISpineSkeleton not initialized \n" );
		return 0;
	}
	self->AddAnimation ( trackId, name, loop, delay );
	return 0;
}

//----------------------------------------------------------------//
/**	@name	clearAllTracks

	@in		MOAISpineSkeleton self
	@out	nil
*/
int MOAISpineSkeleton::_clearAllTracks ( lua_State *L ) {
	MOAI_LUA_SETUP( MOAISpineSkeleton, "U" )
	
	if ( !self->mSkeleton || !self->mAnimationState ) {
		MOAIPrint ( "MOAISpineSkeleton not initialized \n" );
		return 0;
	}
	self->ClearAllTracks ();
	return 0;
}

//----------------------------------------------------------------//
/**	@name	clearTrack

 	@in		MOAISpineSkeleton self
	@in		number track
	@out	nil
*/
int MOAISpineSkeleton::_clearTrack ( lua_State *L ) {
	MOAI_LUA_SETUP( MOAISpineSkeleton, "UN" )
	
	int trackId = state.GetValue < int >( 2, 0);
	
	if ( !self->mSkeleton || !self->mAnimationState ) {
		MOAIPrint ( "MOAISpineSkeleton not initialized \n" );
		return 0;
	}
	self->ClearTrack ( trackId );
	return 0;
}

//----------------------------------------------------------------//
/**	@name	getBone
	@text	Return MOAITransform that is bound to skeleton bone. 
			On first call it will create full hierarchy of MOAISpineBones
			from requested bone to the root bone. It is needed for proper
			transform inheritance.

 	@in		MOAISpineSkeleton self
	@in		string	skeleton bone name
	@out	MOAISpineBone	bone
*/
int MOAISpineSkeleton::_getBone ( lua_State *L ) {
	MOAI_LUA_SETUP ( MOAISpineSkeleton, "US" );
	
	cc8* boneName = state.GetValue < cc8* >( 2, 0 );
	
	spBone* bone = spSkeleton_findBone ( self->mSkeleton, boneName );
	if ( !bone ) {
		return 0;
	}
	
	self->AffirmBoneHierarchy ( bone );
	self->mBoneTransformMap [ boneName ]->PushLuaUserdata ( state );
	return 1;
}

//----------------------------------------------------------------//
/**	@name	getSlot
	@text	Return MOAIColor that is bound to skeleton slot.
			MOAISpineSlots are lazily initialized.

	@in		MOAISpineSkeleton self
	@in		string	skeleton slot name
	@out	MOAISpineSlot	slot
 */
int MOAISpineSkeleton::_getSlot ( lua_State *L ) {
	MOAI_LUA_SETUP ( MOAISpineSkeleton, "US" );
	
	cc8* slotName = state.GetValue < cc8* >( 2, 0 );
	
	spSlot* slot = spSkeleton_findSlot ( self->mSkeleton, slotName );
	if ( !slot ) {
		return 0;
	}
	
	if ( self->mSlotColorMap.contains( slotName ) ) {
		self->mBoneTransformMap [ slotName ]->PushLuaUserdata ( state );
		return 1;
	}
	
	MOAISpineSlot* spineSlot = new MOAISpineSlot();
	spineSlot->SetSlot ( slot );
	self->LuaRetain ( spineSlot );
	self->mSlotColorMap [ slotName ] = spineSlot;
	spineSlot->PushLuaUserdata ( state );
	
	return 1;
}

//----------------------------------------------------------------//
/**	@name	init

 	@in		MOAISpineSkeleton self
	@in		MOAISpineSkeletonData skeleton data
	@out	nil
 
*/
int MOAISpineSkeleton::_init ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAISpineSkeleton, "UU" )
	
	MOAISpineSkeletonData* data = state.GetLuaObject < MOAISpineSkeletonData >( 2, true );
	
	if ( !data->mSkeletonData ) {
		MOAIPrint ( "Empty skeleton data \n" );
		return 0;
	}
	self->mSkeletonData.Set ( *self, data );
	self->Init ( data->mSkeletonData );
	
	return 0;
}

//----------------------------------------------------------------//
/**	@name	initAnimationState

	@in		MOAISpineSkeleton self
	@out	nil
*/
int MOAISpineSkeleton::_initAnimationState ( lua_State *L ) {
	MOAI_LUA_SETUP ( MOAISpineSkeleton, "U" )
	
	if ( !self->mSkeleton ) {
		MOAIPrint ( "MOAISpineSkeleton not initialized \n" );
		return 0;
	}
	spAnimationStateData* data = spAnimationStateData_create ( self->mSkeleton->data );
	self->InitAnimationState ( data );
	
	return 0;
}

//----------------------------------------------------------------//
/**	@name	setAnimation

 	@in		MOAISpineSkeleton self
	@in		number	track Id
	@in		string	animation name
	@in		bool	loop
	@in		number	mix time
	@out	nil
*/
int MOAISpineSkeleton::_setAnimation ( lua_State *L ) {
	MOAI_LUA_SETUP( MOAISpineSkeleton, "UNSBN" )
	
	int trackId = state.GetValue < int >( 2, 0 );
	cc8* name = state.GetValue < cc8* >( 3, "");
	bool loop = state.GetValue < bool >( 4, false );
	float delay = state.GetValue < float >( 5, 0.0f );
	
	if ( !self->mSkeleton || !self->mAnimationState ) {
		MOAIPrint ( "MOAISpineSkeleton not initialized \n" );
		return 0;
	}
	self->SetAnimation ( trackId, name, loop, delay );
	return 0;
}

//----------------------------------------------------------------//
/**	@name	setAttachment
 
 	@in		MOAISpineSkeleton self
	@in		string	slotName
	@in		string	attachmentName
	@out	bool	success
*/
int MOAISpineSkeleton::_setAttachment ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAISpineSkeleton, "USS" );
	
	cc8* slotName = state.GetValue < cc8* >( 2, 0 );
	cc8* attachmentName = state.GetValue < cc8* >( 3, 0 );
	
	if ( !self->mSkeleton ) {
		MOAIPrint ( "MOAISpineSkeleton not initialized \n" );
		state.Push ( false );
		return 1;
	}
	state.Push ( (bool) spSkeleton_setAttachment ( self->mSkeleton, slotName, attachmentName ));
	return 1;
}

//----------------------------------------------------------------//
/**	@name	setBonesToSetupPose

	@in		MOAISpineSkeleton self
	@out	nil
*/
int MOAISpineSkeleton::_setBonesToSetupPose	( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAISpineSkeleton, "U" );
	
	if ( !self->mSkeleton ) {
		MOAIPrint ( "MOAISpineSkeleton not initialized \n" );
		return 0;
	}
	spSkeleton_setBonesToSetupPose ( self->mSkeleton );
	self->UpdateSkeleton ();
	return 0;
}

//----------------------------------------------------------------//
/**	@name	setFlip
 
	@in		MOAISpineSkeleton self
	@in		bool	flipX
	@in		bool	flipY
	@out	nil
 */
int MOAISpineSkeleton::_setFlip ( lua_State *L ) {
	MOAI_LUA_SETUP ( MOAISpineSkeleton, "U" );
	
	bool flipX = state.GetValue < bool >( 2, false );
	bool flipY = state.GetValue < bool >( 3, false );
	
	if ( !self->mSkeleton ) {
		MOAIPrint ( "MOAISpineSkeleton not initialized \n" );
		return 0;
	}
	self->mSkeleton->flipX = flipX;
	self->mSkeleton->flipY = flipY;
	
	BoneTransformIt it = self->mBoneTransformMap.begin ();
	for ( ; it != self->mBoneTransformMap.end (); ++it ) {
		it->second->mFlipX = flipX;
		it->second->mFlipY = flipY;
	}
	
	return 0;
}

//----------------------------------------------------------------//
/**	@name	setMix
 
 	@in		MOAISpineSkeleton self
	@in		string	from
	@in		string	to
	@in		number	duration
	@out	nil
*/
int MOAISpineSkeleton::_setMix ( lua_State *L ) {
	MOAI_LUA_SETUP ( MOAISpineSkeleton, "USSN" );
	
	cc8* fromName = state.GetValue < cc8* >( 2, "" );
	cc8* toName   = state.GetValue < cc8* >( 3, "" );
	float duration  = state.GetValue < float >( 4, 0.1 );
	
	if ( !self->mSkeleton || !self->mAnimationState ) {
		MOAIPrint ( "MOAISpineSkeleton not initialized \n" );
		return 0;
	}
	self->SetMix( fromName, toName, duration );
	
	return 0;
}

//----------------------------------------------------------------//
/**	@name	setSkin

 	@in		MOAISpineSkeleton self
	@in		string skin name
	@out	nil
*/
int MOAISpineSkeleton::_setSkin	( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAISpineSkeleton, "US" )
	
	cc8* skinName = state.GetValue < cc8* >( 2, 0 );
	
	if ( !self->mSkeleton ) {
		MOAIPrint ( "MOAISpineSkeleton not initialized \n" );
		return 0;
	}
	spSkeleton_setSkinByName ( self->mSkeleton, skinName );
	spSkeleton_setSlotsToSetupPose( self->mSkeleton );
	return 0;
}

//----------------------------------------------------------------//
/**	@name	setSlotsToSetupPose

	@in		MOAISpineSkeleton self
	@out	nil
*/
int MOAISpineSkeleton::_setSlotsToSetupPose	( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAISpineSkeleton, "U" );
	
	if ( !self->mSkeleton ) {
		MOAIPrint ( "MOAISpineSkeleton not initialized \n" );
		return 0;
	}
	spSkeleton_setSlotsToSetupPose ( self->mSkeleton );
	return 0;
}

//----------------------------------------------------------------//
/**	@name	setToSetupPose
	
	@in		MOAISpineSkeleton self
	@out	nil
*/
int MOAISpineSkeleton::_setToSetupPose ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAISpineSkeleton, "U" );
	
	if ( !self->mSkeleton ) {
		MOAIPrint ( "MOAISpineSkeleton not initialized \n" );
		return 0;
	}
	spSkeleton_setToSetupPose ( self->mSkeleton );
	self->UpdateSkeleton ();
	return 0;
}

//================================================================//
// MOAISpineSkeleton
//================================================================//

//----------------------------------------------------------------//
void MOAISpineSkeleton::AddAnimation ( int trackId, cc8* name, bool loop, float delay ) {
	spAnimation* anim = spSkeletonData_findAnimation ( mSkeleton->data, name );
	
	assert ( anim );
	spAnimationState_addAnimation ( mAnimationState, trackId, anim, loop, delay );
}

//----------------------------------------------------------------//
void MOAISpineSkeleton::AffirmBoneHierarchy ( spBone* bone ) {
	// create all missing MOAISpineBones first
	for ( spBone* boneIt = bone ; boneIt; boneIt = boneIt->parent ) {
		if ( mBoneTransformMap.contains ( boneIt->data->name )) {
			continue;
		}
		
		MOAISpineBone* luaBone = new MOAISpineBone();
		luaBone->SetBone ( boneIt );
		luaBone->mFlipX = mSkeleton->flipX;
		luaBone->mFlipY = mSkeleton->flipY;
		this->LuaRetain ( luaBone );
		mBoneTransformMap [ boneIt->data->name ] = luaBone;
		
		if ( boneIt == mSkeleton->bones [ 0 ] ) {
			mRootBone = luaBone;
			luaBone->SetAsRootBone ( this );
		}
	}
	
	// create attr links
	for ( spBone* boneIt = bone ; boneIt->parent; boneIt = boneIt->parent ) {
		MOAISpineBone* curBone = mBoneTransformMap [ boneIt->data->name ];
		MOAISpineBone* parent = mBoneTransformMap [ boneIt->parent->data->name ];
		curBone->SetAttrLink ( PACK_ATTR ( MOAITransform, INHERIT_TRANSFORM ), parent, PACK_ATTR ( MOAITransformBase, TRANSFORM_TRAIT ));
	}
}

//----------------------------------------------------------------//
void MOAISpineSkeleton::ClearAllTracks () {
	spAnimationState_clearTracks ( mAnimationState );
}

//----------------------------------------------------------------//
void MOAISpineSkeleton::ClearTrack ( int trackId ) {
	spAnimationState_clearTrack ( mAnimationState, trackId );
}

//----------------------------------------------------------------//
void MOAISpineSkeleton::Draw ( int subPrimID ) {
	UNUSED ( subPrimID );
	
	if ( !this->IsVisible () ) return;
		
	MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get ();
	
	if ( this->mUVTransform ) {
		ZLAffine3D uvMtx = this->mUVTransform->GetLocalToWorldMtx ();
		gfxDevice.SetUVTransform ( uvMtx );
	}
	else {
		gfxDevice.SetUVTransform ();
	}
	
	this->LoadGfxState ();
	
	if ( !this->mShader ) {
		gfxDevice.SetShaderPreset ( MOAIShaderMgr::DECK2D_SHADER );
	}
	
	gfxDevice.SetVertexTransform ( MOAIGfxDevice::VTX_WORLD_TRANSFORM, this->GetLocalToWorldMtx ());
	gfxDevice.SetVertexMtxMode ( MOAIGfxDevice::VTX_STAGE_MODEL, MOAIGfxDevice::VTX_STAGE_PROJ );
	gfxDevice.SetUVMtxMode ( MOAIGfxDevice::UV_STAGE_MODEL, MOAIGfxDevice::UV_STAGE_TEXTURE );

	MOAIQuadBrush::BindVertexFormat ( gfxDevice );
	
	MOAIBlendMode normal;
	MOAIBlendMode additive;
	normal.SetBlend ( MOAIBlendMode::BLEND_NORMAL );
	additive.SetBlend ( MOAIBlendMode::BLEND_ADD );

	u32 size = this->mQuads.Size ();
	for ( u32 i = 0; i < size; ++i ) {
		spSlot* slot = mSkeleton->drawOrder [ i ];
		
		if ( !slot->attachment || slot->attachment->type != SP_ATTACHMENT_REGION)
			continue;
		
		spRegionAttachment *attachment = (spRegionAttachment*) slot->attachment;
		MOAITexture* texture = (MOAITexture*) ((spAtlasRegion*) attachment->rendererObject)->page->rendererObject;
		
		gfxDevice.SetTexture ( texture );
		
		if ( slot->data->additiveBlending ) {
			gfxDevice.SetBlendMode ( additive );
		}
		else {
			gfxDevice.SetBlendMode ( normal );
		}
		
		float a = slot->skeleton->a * slot->a;
		float r = slot->skeleton->r * slot->r;
		float g = slot->skeleton->g * slot->g;
		float b = slot->skeleton->b * slot->b;
		
		ZLColorVec slotColor (r, g, b, a);
		ZLColorVec baseColor = this->mColor;
		slotColor.Modulate ( baseColor );
		gfxDevice.SetPenColor ( slotColor );
		
		MOAIQuadBrush& quad = mQuads [ i ];
		quad.Draw ();
	}
}

//----------------------------------------------------------------//
void MOAISpineSkeleton::DrawDebug ( int subPrimID ) {
	MOAIProp::DrawDebug ( subPrimID );
	
	
}

//----------------------------------------------------------------//
u32 MOAISpineSkeleton::GetPropBounds ( ZLBox &bounds ) {
	
	if ( this->mFlags & FLAGS_OVERRIDE_BOUNDS ) {
		bounds = this->mBoundsOverride;
		return BOUNDS_OK;
	}
	
	u32 size = mSkeleton->slotCount;
	
	if ( size == 0) {
		return MOAIProp::BOUNDS_EMPTY;
	}
	
	this->UpdateBoundsAndQuads ();
	bounds.Init ( mSkeletonBounds );
	
	return MOAIProp::BOUNDS_OK;
}

//----------------------------------------------------------------//
void MOAISpineSkeleton::Init ( spSkeletonData *skeletonData ) {
	
	mSkeleton = spSkeleton_create ( skeletonData );
	
	u32 total = mSkeleton->slotCount;
	mQuads.Init ( total );
	
	this->UpdateSkeleton ();
	this->UpdateBoundsAndQuads ();
}

//----------------------------------------------------------------//
void MOAISpineSkeleton::InitAnimationState ( spAnimationStateData *animData ) {
	
	mAnimationState = spAnimationState_create ( animData );
	mAnimationState->rendererObject = this;
	mAnimationState->listener = callback;
}

//----------------------------------------------------------------//
bool MOAISpineSkeleton::IsDone () {
	return false;
}

//----------------------------------------------------------------//
MOAISpineSkeleton::MOAISpineSkeleton ():
	mDebugSlots ( false ),
    mDebugBones ( false ),
    mBoundsDirty ( true ),
    mSkeleton ( 0 ),
    mAnimationState ( 0 ),
    mRootBone ( 0 ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIProp )
		RTTI_EXTEND ( MOAIAction )
	RTTI_END
}

//----------------------------------------------------------------//
MOAISpineSkeleton::~MOAISpineSkeleton () {
	mQuads.Clear ();
	
	if ( mRootBone ) {
		mRootBone->SetAsRootBone ( 0 );
	}
	
	for ( BoneTransformIt it = mBoneTransformMap.begin (); it != mBoneTransformMap.end (); ++it ) {
		it->second->SetBone ( 0 );
		this->LuaRelease( it->second );
	}
	mBoneTransformMap.clear ();
	
	for ( SlotColorIt it = mSlotColorMap.begin (); it != mSlotColorMap.end (); ++it ) {
		it->second->SetSlot ( 0 );
		this->LuaRelease( it->second );
	}
	mSlotColorMap.clear ();
	
	if ( mAnimationState ) {
		spAnimationStateData_dispose ( mAnimationState->data );
		spAnimationState_dispose ( mAnimationState );
	}
	
	if ( mSkeleton ) {
		spSkeleton_dispose ( mSkeleton );
	}
	
	mSkeletonData.Set ( *this, 0 );
}

//----------------------------------------------------------------//
void MOAISpineSkeleton::OnAnimationEvent ( int trackIndex, spEventType type, spEvent* event, int loopCount ) {
	MOAIScopedLuaState state = MOAILuaRuntime::Get ().State ();
	switch ( type ) {
		case SP_ANIMATION_START:
			if ( this->PushListenerAndSelf ( EVENT_ANIMATION_START, state) ) {
				state.Push ( trackIndex );
				state.DebugCall ( 2, 0 );
			}
			break;
		
		case SP_ANIMATION_END:
			if ( this->PushListenerAndSelf ( EVENT_ANIMATION_END, state) ) {
				state.Push ( trackIndex );
				state.DebugCall ( 2, 0 );
			}
			break;
			
		case SP_ANIMATION_COMPLETE:
			if ( this->PushListenerAndSelf ( EVENT_ANIMATION_COMPLETE, state) ) {
				state.Push ( trackIndex );
				state.Push ( loopCount );
				state.DebugCall ( 3, 0 );
			}
			break;
			
		case SP_ANIMATION_EVENT:
			if ( this->PushListenerAndSelf ( EVENT_ANIMATION_EVENT, state) ) {
				state.Push ( trackIndex );
				state.Push ( event->data->name );
				state.Push ( event->intValue );
				state.Push ( event->floatValue );
				state.Push ( event->stringValue );
				state.DebugCall ( 6, 0 );
			}
			break;
	}
}

//----------------------------------------------------------------//
void MOAISpineSkeleton::OnDepNodeUpdate () {
	
	// Skeleton should be updated before prop for correct bounds
	this->UpdateSkeleton ();
	
	MOAIProp::OnDepNodeUpdate ();
}

//----------------------------------------------------------------//
void MOAISpineSkeleton::OnUpdate ( float step ) {
	if ( mSkeleton ) {
		spSkeleton_update ( mSkeleton, step );
		
		if ( mAnimationState ) {
			spAnimationState_update ( mAnimationState, step );
			spAnimationState_apply ( mAnimationState, mSkeleton );
		}
		
		if ( mRootBone ) {
			mRootBone->ScheduleUpdate ();
		}
		
		for ( SlotColorIt it = mSlotColorMap.begin (); it != mSlotColorMap.end (); ++it ) {
			it->second->ScheduleUpdate ();
		}
		
		this->ScheduleUpdate ();
	}
}

//----------------------------------------------------------------//
void MOAISpineSkeleton::RegisterLuaClass ( MOAILuaState& state ) {

	MOAIProp::RegisterLuaClass ( state );
	MOAIAction::RegisterLuaClass ( state );
	
	state.SetField ( -1, "EVENT_ANIMATION_START", ( u32 )EVENT_ANIMATION_START );
	state.SetField ( -1, "EVENT_ANIMATION_END", ( u32 )EVENT_ANIMATION_END );
	state.SetField ( -1, "EVENT_ANIMATION_COMPLETE", ( u32 )EVENT_ANIMATION_COMPLETE );
	state.SetField ( -1, "EVENT_ANIMATION_EVENT", ( u32 )EVENT_ANIMATION_EVENT );
}

//----------------------------------------------------------------//
void MOAISpineSkeleton::RegisterLuaFuncs ( MOAILuaState& state ) {

	MOAIProp::RegisterLuaFuncs ( state );
	MOAIAction::RegisterLuaFuncs ( state );

	luaL_Reg regTable [] = {
		{ "addAnimation", 			_addAnimation },
		{ "clearAllTracks", 		_clearAllTracks },
		{ "clearTrack", 			_clearTrack },
		{ "getBone",				_getBone },
		{ "getSlot",				_getSlot },
		{ "init", 					_init },
		{ "initAnimationState", 	_initAnimationState },
		{ "setAnimation", 			_setAnimation },
		{ "setAttachment", 			_setAttachment },
		{ "setBonesToSetupPose", 	_setBonesToSetupPose },
		{ "setFlip", 				_setFlip },
		{ "setMix", 				_setMix },
		{ "setSkin", 				_setSkin },
		{ "setSlotsToSetupPose", 	_setSlotsToSetupPose },
		{ "setToSetupPose", 		_setToSetupPose },
		{ NULL, NULL }
	};
	
	luaL_register ( state, 0, regTable );
}

//----------------------------------------------------------------//
void MOAISpineSkeleton::SetAnimation ( int trackId, cc8* name, bool loop, float delay ) {
	spAnimation* anim = spSkeletonData_findAnimation ( mSkeleton->data, name );
	
	assert ( anim );
	spAnimationState_setAnimation ( mAnimationState, trackId, anim, loop);
}

//----------------------------------------------------------------//
void MOAISpineSkeleton::SetMix ( cc8* fromName, cc8* toName, float duration ) {
	spAnimation* fromAnim = spSkeletonData_findAnimation ( mSkeleton->data, fromName );
	spAnimation* toAnim = spSkeletonData_findAnimation ( mSkeleton->data, toName );
	
	assert ( fromAnim );
	assert ( toAnim );
	spAnimationStateData_setMix ( mAnimationState->data, fromAnim, toAnim, duration );
}

//----------------------------------------------------------------//
void MOAISpineSkeleton::UpdateBoundsAndQuads () {
	if ( !mBoundsDirty ) {
		return;
	}
	mBoundsDirty = false;
	
	float minX = FLT_MAX, minY = FLT_MAX, maxX = -FLT_MAX, maxY = -FLT_MAX;
	float vertices[8];
	
	u32 size = this->mQuads.Size ();
	for ( u32 i = 0; i < size; ++i ) {
		spSlot* slot = mSkeleton->drawOrder [ i ];
		
		if ( !slot->attachment || slot->attachment->type != SP_ATTACHMENT_REGION)
			continue;
		
		spRegionAttachment *attachment = (spRegionAttachment*) slot->attachment;
		spRegionAttachment_computeWorldVertices ( attachment, slot->skeleton->x, slot->skeleton->y, slot->bone, vertices );
		
		MOAIQuadBrush& quad = mQuads [ i ];
		quad.SetVerts ( vertices );
		quad.SetUVs ( attachment->uvs );
		
		minX = fmin ( minX, vertices[SP_VERTEX_X1] );
		minY = fmin ( minY, vertices[SP_VERTEX_Y1] );
		maxX = fmax ( maxX, vertices[SP_VERTEX_X1] );
		maxY = fmax ( maxY, vertices[SP_VERTEX_Y1] );
		minX = fmin ( minX, vertices[SP_VERTEX_X4] );
		minY = fmin ( minY, vertices[SP_VERTEX_Y4] );
		maxX = fmax ( maxX, vertices[SP_VERTEX_X4] );
		maxY = fmax ( maxY, vertices[SP_VERTEX_Y4] );
		minX = fmin ( minX, vertices[SP_VERTEX_X2] );
		minY = fmin ( minY, vertices[SP_VERTEX_Y2] );
		maxX = fmax ( maxX, vertices[SP_VERTEX_X2] );
		maxY = fmax ( maxY, vertices[SP_VERTEX_Y2] );
		minX = fmin ( minX, vertices[SP_VERTEX_X3] );
		minY = fmin ( minY, vertices[SP_VERTEX_Y3] );
		maxX = fmax ( maxX, vertices[SP_VERTEX_X3] );
		maxY = fmax ( maxY, vertices[SP_VERTEX_Y3] );
	}
	
	mSkeletonBounds.Init ( minX, maxY, maxX, minY, 0.f, 0.f );
}

//----------------------------------------------------------------//
void MOAISpineSkeleton::UpdateSkeleton () {
	
	if ( !mSkeleton )
		return;
	
	spSkeleton_updateWorldTransform ( mSkeleton );
	mBoundsDirty = true;
}



