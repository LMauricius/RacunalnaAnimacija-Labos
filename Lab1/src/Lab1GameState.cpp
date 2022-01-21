
#include "Lab1GameState.h"
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
	Lab1GameState::Lab1GameState( const Ogre::String &helpDescription ) :
		TutorialGameState( helpDescription )
	{
    }
    //-----------------------------------------------------------------------------------
	void Lab1GameState::createScene01(void)
    {
        mCameraController = new CameraController( mGraphicsSystem, false );

        TutorialGameState::createScene01();
        Ogre::SceneManager *mgr = mGraphicsSystem->getSceneManager();

        // ***
        // *** MAIN OBJECT ***
        // ***
        mObj = make_unique<MainObject>();
        mObj->init(mgr, "Griffon.mesh");

        // ***
        // *** CURVE ***
        // ***
        mCurve = make_unique<Curve>();

        {
            std::ifstream f("../../models/path.txt");
            std::string ln;
            while (std::getline(f, ln))
            {
                auto p = Ogre::StringConverter::parseVector3(ln);
                mCurve->addPoint(p);
            }
            f.close();
        }

        mCurve->init(mgr, "Sphere1000.mesh");
        splinePos = 0.0;

        // ***
        // *** TANGENT ***
        // ***
        Ogre::Item *tanItem = mgr->createItem(createLineMesh("tangent.mesh", {Ogre::Vector3::ZERO, Ogre::Vector3::UNIT_Y}));
        tangentNode = mgr->getRootSceneNode()->createChildSceneNode();
        tangentNode->attachObject(tanItem);

        // ***
        // *** LIGHTS ***
        // ***
        Ogre::SceneNode *ln = mgr->getRootSceneNode()->createChildSceneNode();
        Ogre::Light *l = mgr->createLight();
        ln->attachObject(l);
        l->setType(Ogre::Light::LightTypes::LT_DIRECTIONAL);
        l->setDiffuseColour(Ogre::ColourValue::White);
    }
	void Lab1GameState::destroyScene(void)
    {
        mObj->destroy();
        mCurve->destroy();
        TutorialGameState::destroyScene();
    }
    //-----------------------------------------------------------------------------------
	void Lab1GameState::update( float timeSinceLast )
	{
        splinePos += moveSpeedMod * moveSpeed * timeSinceLast * 0.3;
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
        tangentNode->setScale(Ogre::Vector3(der.length()));

        TutorialGameState::update( timeSinceLast );
    }
    //-----------------------------------------------------------------------------------
	void Lab1GameState::generateDebugText( float timeSinceLast, Ogre::String &outText )
	{
		TutorialGameState::generateDebugText( timeSinceLast, outText );
        
        outText += "\nPress +/- to move. ";
        outText += "[" + Ogre::StringConverter::toString(splinePos) + "]";
	}
    //-----------------------------------------------------------------------------------
	void Lab1GameState::keyPressed( const SDL_KeyboardEvent &arg )
    {
        switch (arg.keysym.scancode)
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
        }

		TutorialGameState::keyPressed( arg );
    }
    //-----------------------------------------------------------------------------------
	void Lab1GameState::keyReleased( const SDL_KeyboardEvent &arg )
    {
        switch (arg.keysym.scancode)
        {
            case SDL_SCANCODE_LSHIFT:
            case SDL_SCANCODE_RSHIFT:
                moveSpeedMod = 1;
                break;
            case SDL_SCANCODE_KP_PLUS:
            case SDL_SCANCODE_KP_MINUS:
                moveSpeed = 0.0;
                break;
        }

        if( (arg.keysym.mod & ~(KMOD_NUM|KMOD_CAPS)) != 0 )
        {
            TutorialGameState::keyReleased( arg );
            return;
        }

		TutorialGameState::keyReleased( arg );
    }
}
