/*
 *  Well.cpp
 *  GravitySwarm
 *
 *  Created by Mario.Gonzalez on 6/15/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include "cinder/Rand.h"
#include "cinder/Vector.h"
using namespace ci;

class Well {
public:
	Well( Vec2f aPosition, uint32_t touchID )
	{
		mPosition = aPosition;
		mTouchPosition = aPosition;
		mLastTouchPosition = aPosition;
		mTouchID = touchID;
		
		mStrength = Rand::randFloat(11.0, 12.0f);
		mStrengthSquared = mStrength * mStrength;
		
		minDistance = 45.0f;
		minDistanceSquared = minDistance * minDistance;
		
		mLifespan = Rand::randInt(200, 650);
		mAge = 0;
		mAgePer = 0.0f;
		mIsDead = false;
		
		mHue	= Rand::randFloat();
		mForceScale = 1.0f;
	};
	
	Vec2f mPosition, mTouchPosition, mLastTouchPosition;
	
	float mStrength;
	float mForceScale;
	float mStrengthSquared;
	
	float minDistance;
	float minDistanceSquared;
	
	int mLifespan;
	int mAge;
	bool mIsDead;
	float mAgePer;
	
	float mHue;
	
	uint32_t mTouchID;
};