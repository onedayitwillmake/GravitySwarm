/*
 *  Particle.cpp
 *  GravitySwarm
 *
 *  Created by Mario.Gonzalez on 6/15/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "cinder/Rand.h"
#include "cinder/Vector.h"
using namespace ci;

class Particle
{
public:
	Particle( Vec2f aPosition, float aForceScale )
	{
		mPosition = aPosition;
		mLastPosition = aPosition;
		mLastLastPosition = aPosition;
		reset();
	}
	
	void reset()
	{
		mVelocity = Vec2f::zero();
		noise = 0.0f;
		
		gravityForceScale = Rand::randFloat(1.0f, 1.5f);
		damping = Rand::randFloat(0.985f, 0.985f);
		maxLinearVelocity = 12.0f;//Rand::randFloat(10.0f, 12.0f);
		
		mLifespan = Rand::randInt(40, 250);
		
		mAge = 1;
		mAgePer = 0;
		mIsDead = false;
	}
	
	void reset(Vec2f aPosition)
	{
		mLastPosition = aPosition;
		mLastLastPosition = aPosition;
		mPosition = aPosition;
		reset();
	}
	
	void tick()
	{
		mLastLastPosition = mLastPosition;
		mLastPosition = mPosition;
		mAgePer = 1.0f - ( mAge / (float) mLifespan );
		
		// cap the velocity 
		float maxVel = maxLinearVelocity;
		float velLength = mVelocity.lengthSquared() + 0.1f;
		if( velLength > maxVel*maxVel )
		{
			mVelocity.normalize();
			mVelocity *= maxVel;
		}
		
		// Move the particles according to their velocities, then dampen
		mPosition += mVelocity;
		mVelocity *= damping;
	}
	
	Vec2f mPosition, mVelocity, mLastPosition, mLastLastPosition;
	
	int mLifespan;
	int mAge;
	float mAgePer;
	bool mIsDead;
	
	float maxLinearVelocity;
	float damping;
	float noise;
	
	float gravityForceScale;
};

