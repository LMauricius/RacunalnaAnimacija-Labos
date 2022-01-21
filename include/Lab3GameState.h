
#ifndef _Demo_EmptyProjectGameState_H_
#define _Demo_EmptyProjectGameState_H_

#include "OgrePrerequisites.h"
#include "TutorialGameState.h"
#include "Character.h"
#include "ptrs.h"

#include "SDL_scancode.h"

#include <map>

namespace Demo
{
	class Lab3GameState : public TutorialGameState
    {
        virtual void generateDebugText( float timeSinceLast, Ogre::String &outText );

        Character *mCharacter;
        Ogre::Light *mLight;

        Ogre::SceneNode *targetNode;
        Ogre::Vector3 targetVelocity;
        float targetSpeed;
        Ogre::StringVector armAnimNames;
        Ogre::StringVector footAnimNames;
        float legAdjustHeight;
        
        Ogre::SceneNode *floorNode;
        Ogre::Vector3 floorNormalAndHeight;

        enum SelectedObj 
        {
            Target,
            Floor
        } selectedObj;
        /*uptr<MainObject> mObj;
        uptr<Curve> mCurve;
        Ogre::SceneNode *tangentNode;
        double splinePos;
        double moveSpeed = 0.0;
        double moveSpeedMod = 1.0;*/
        std::map<std::string, SDL_Scancode> mAnimKeys;

    public:
		Lab3GameState( const Ogre::String &helpDescription );

        virtual void createScene01(void);
        virtual void destroyScene(void);
        
        virtual void loadSceneConfig(void);

        virtual void update( float timeSinceLast );

        virtual void keyPressed( const SDL_KeyboardEvent &arg ) override;
        virtual void keyReleased( const SDL_KeyboardEvent &arg );
    };
}

#endif
