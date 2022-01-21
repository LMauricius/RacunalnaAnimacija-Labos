
#include "Lab2GameState.h"
#include "CameraController.h"
#include "GraphicsSystem.h"

#include "OgreSceneManager.h"
#include "OgreItem.h"

#include "OgreMeshManager.h"
#include "OgreMeshManager2.h"
#include "OgreMesh2.h"

#include "OgreCamera.h"
#include "OgreWindow.h"

#include "OgreHlmsPbsDatablock.h"
#include "OgreHlmsSamplerblock.h"

#include "OgreRoot.h"
#include "OgreHlmsManager.h"
#include "OgreHlmsPbs.h"

#include "MeshCreationUtil.h"

#include <fstream>

using namespace Demo;

namespace Demo
{
	Lab2GameState::Lab2GameState( const Ogre::String &helpDescription ) :
		TutorialGameState( helpDescription )
	{
    }
    //-----------------------------------------------------------------------------------
	void Lab2GameState::createScene01(void)
    {
        mCameraController = new CameraController( mGraphicsSystem, false );

        TutorialGameState::createScene01();
        Ogre::SceneManager *mgr = mGraphicsSystem->getSceneManager();

        // ***
        // *** MAIN OBJECT ***
        // ***
        partSys = new MyParticleSys(mgr);
        partSys->setMaterial("iskrica");
        partSys->setOrigin(Ogre::Vector3(0, 3, 0));
        partSys->setDimensions(2, 2);
        partSys->setParams(
            5, 5,
            3.5, 3.5,
            1.5, 4.5,
            Ogre::Degree(90), Ogre::Degree(270),
            Ogre::Vector3::UNIT_Z, Ogre::Degree(0), Ogre::Degree(45),
            true
        );
        /*partSys->addPlane(Ogre::Plane(
            Ogre::Vector3::UNIT_Y,
            Ogre::Vector3(0,-2, 0)
        ));*/
        partSys->init();

        // ***
        // *** LIGHTS ***
        // ***
        Ogre::SceneNode *ln = mgr->getRootSceneNode()->createChildSceneNode();
        Ogre::Light *l = mgr->createLight();
        ln->attachObject(l);
        l->setType(Ogre::Light::LightTypes::LT_DIRECTIONAL);
        l->setDiffuseColour(Ogre::ColourValue::White);
    }
	void Lab2GameState::destroyScene(void)
    {
        partSys->destroy();
        delete partSys;
        TutorialGameState::destroyScene();
    }
    //-----------------------------------------------------------------------------------
	void Lab2GameState::update( float timeSinceLast )
	{
        partSys->update(timeSinceLast, mGraphicsSystem->getCamera());
        /*splinePos += moveSpeedMod * moveSpeed * timeSinceLast * 0.3;
        Ogre::Vector3 p = mCurve->getPosition(splinePos);
        Ogre::Vector3 der = mCurve->getDerivation(splinePos);
        Ogre::Vector3 dir = der.normalisedCopy();
        Ogre::Vector3 start = Ogre::Vector3::UNIT_Y;
        Ogre::Vector3 axis = start.crossProduct(dir);
        Ogre::Radian angle = Ogre::Math::ACos(start.dotProduct(dir)/start.length()/dir.length());
        Ogre::Quaternion rot = Ogre::Quaternion(angle, axis);

        mObj->mNode->setPosition(p);
        mObj->mNode->setOrientation(rot);

        tangentNode->setPosition(p);
        tangentNode->setOrientation(rot);
        tangentNode->setScale(Ogre::Vector3(der.length()));*/

        TutorialGameState::update( timeSinceLast );
    }
    //-----------------------------------------------------------------------------------
	void Lab2GameState::generateDebugText( float timeSinceLast, Ogre::String &outText )
	{
		TutorialGameState::generateDebugText( timeSinceLast, outText );
        
        /*outText += "\nPress +/- to move. ";
        outText += "[" + Ogre::StringConverter::toString(splinePos) + "]";*/
	}
    //-----------------------------------------------------------------------------------
	void Lab2GameState::keyPressed( const SDL_KeyboardEvent &arg )
    {
        /*switch (arg.keysym.scancode)
        {
            case SDL_SCANCODE_LSHIFT:
            case SDL_SCANCODE_RSHIFT:
                moveSpeedMod = 5;
                break;
            case SDL_SCANCODE_KP_PLUS:
                moveSpeed = 1.0;
                break;
            case SDL_SCANCODE_KP_MINUS:
                moveSpeed = -1.0;
                break;
        }*/

		TutorialGameState::keyPressed( arg );
    }
    //-----------------------------------------------------------------------------------
	void Lab2GameState::keyReleased( const SDL_KeyboardEvent &arg )
    {
        /*switch (arg.keysym.scancode)
        {
            case SDL_SCANCODE_LSHIFT:
            case SDL_SCANCODE_RSHIFT:
                moveSpeedMod = 1;
                break;
            case SDL_SCANCODE_KP_PLUS:
            case SDL_SCANCODE_KP_MINUS:
                moveSpeed = 0.0;
                break;
        }*/

        if( (arg.keysym.mod & ~(KMOD_NUM|KMOD_CAPS)) != 0 )
        {
            TutorialGameState::keyReleased( arg );
            return;
        }

		TutorialGameState::keyReleased( arg );
    }
}
