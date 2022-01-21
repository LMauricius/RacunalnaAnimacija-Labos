
#ifndef _Demo_EmptyProjectGameState_H_
#define _Demo_EmptyProjectGameState_H_

#include "OgrePrerequisites.h"
#include "TutorialGameState.h"
#include "MainObject.h"
#include "Curve.h"
#include "MyParticleSys.h"
#include "ptrs.h"

namespace Demo
{
	class Lab2GameState : public TutorialGameState
    {
        virtual void generateDebugText( float timeSinceLast, Ogre::String &outText );

        MyParticleSys *partSys;
        /*uptr<MainObject> mObj;
        uptr<Curve> mCurve;
        Ogre::SceneNode *tangentNode;
        double splinePos;
        double moveSpeed = 0.0;
        double moveSpeedMod = 1.0;*/

    public:
		Lab2GameState( const Ogre::String &helpDescription );

        virtual void createScene01(void);
        virtual void destroyScene(void);

        virtual void update( float timeSinceLast );

        virtual void keyPressed( const SDL_KeyboardEvent &arg ) override;
        virtual void keyReleased( const SDL_KeyboardEvent &arg );
    };
}

#endif
