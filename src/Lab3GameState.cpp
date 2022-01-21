
#include "Lab3GameState.h"
#include "CameraController.h"
#include "GraphicsSystem.h"

#include "OgreSceneManager.h"
#include "OgreItem.h"
#include "OgreOldBone.h"
#include "OgreConfigFile.h"

#include "OgreCamera.h"
#include "OgreWindow.h"

#include "OgreRoot.h"
#include "OgreMeshManager.h"
#include "OgreHlmsManager.h"
#include "OgreHlmsPbs.h"

#include "MeshCreationUtil.h"

#include <fstream>

using namespace Demo;

namespace Demo
{
	Lab3GameState::Lab3GameState( const Ogre::String &helpDescription ) :
		TutorialGameState( helpDescription )
	{
    }
    //-----------------------------------------------------------------------------------
	void Lab3GameState::createScene01(void)
    {
        mCameraController = new CameraController( mGraphicsSystem, false );

        TutorialGameState::createScene01();
        Ogre::SceneManager *mgr = mGraphicsSystem->getSceneManager();

        // ***
        // *** MAIN OBJECT ***
        // ***
        mCharacter = new Character();
        mCharacter->init(mgr);

        SDL_Scancode curScancode = SDL_SCANCODE_KP_1;
        for (auto& aname : mCharacter->animController->getAnimNames())
        {
            auto a = mCharacter->animController->getAnim(aname);
            mAnimKeys[aname] = curScancode;
            curScancode = (SDL_Scancode)((int)curScancode+1);
        }

        // ***
        // *** Target object ***
        // ***
        targetNode = mgr->getRootSceneNode()->createChildSceneNode();
        Ogre::Item *targetItem = mgr->createItem("Sphere1000.mesh");
        //Ogre::SceneNode *node = mMgr->getRootSceneNode()->createChildSceneNode();
        targetNode->attachObject(targetItem);
        targetNode->setPosition(Ogre::Vector3::ZERO);
        targetNode->setScale(Ogre::Vector3(0.08));
        targetVelocity = Ogre::Vector3::ZERO;

        // ***
        // *** Floor object ***
        // ***
        Ogre::v1::MeshManager::getSingleton().createPlane(
            "ground.mesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Ogre::Plane(Ogre::Vector3::UNIT_Y, Ogre::Vector3::ZERO),
            50, 50, 3, 3, true,
            1, 1.0, 1.0, Ogre::Vector3::UNIT_Z
        );
        floorNormalAndHeight = Ogre::Vector3::ZERO;
        floorNode = mgr->getRootSceneNode()->createChildSceneNode();
        Ogre::v1::Entity *floorEnt = mgr->createEntity("ground.mesh");
        floorEnt->setDatablockOrMaterialName("Floor");
        //Ogre::SceneNode *node = mMgr->getRootSceneNode()->createChildSceneNode();
        floorNode->attachObject(floorEnt);
        floorNode->setPosition(Ogre::Vector3::ZERO);
        floorNode->setScale(Ogre::Vector3(0.08));

        selectedObj = Target;

        // ***
        // *** LIGHTS ***
        // ***
        Ogre::SceneNode *ln = mgr->getRootSceneNode()->createChildSceneNode();
        mLight = mgr->createLight();
        ln->attachObject(mLight);
        mLight->setType(Ogre::Light::LightTypes::LT_DIRECTIONAL);

        loadSceneConfig();
    }
    void Lab3GameState::loadSceneConfig()
    {
        Ogre::SceneManager *mgr = mGraphicsSystem->getSceneManager();

        Ogre::ConfigFile cf;
        cf.load("../../models/sceneParams.txt");
        mLight->setDiffuseColour(Ogre::StringConverter::parseColourValue(cf.getSetting("LightColor")));
        mLight->setSpecularColour(Ogre::StringConverter::parseColourValue(cf.getSetting("LightColor")));
        mLight->setDirection(Ogre::StringConverter::parseVector3(cf.getSetting("LightDir")).normalisedCopy());
        mgr->setAmbientLight(
            Ogre::StringConverter::parseColourValue(cf.getSetting("Ambient")),
            Ogre::StringConverter::parseColourValue(cf.getSetting("Ambient")),
            Ogre::Vector3::UNIT_Y
        );
        targetSpeed = Ogre::StringConverter::parseReal(cf.getSetting("TargetSpeed"));
        armAnimNames = Ogre::StringConverter::parseStringVector(cf.getSetting("ArmAnimations"));
        footAnimNames = Ogre::StringConverter::parseStringVector(cf.getSetting("LegAnimations"));
        legAdjustHeight = Ogre::StringConverter::parseReal(cf.getSetting("LegAdjustHeight"));
        
        for (auto& name : footAnimNames)
        {
            if (auto ikAnim = dynamic_cast<AnimSystem::IKAnim*>(mCharacter->animController->getAnim(name).get()))
            {
                ikAnim->targetAdjustFunction = [this](
                    Ogre::v1::Entity *ent, Ogre::v1::OldBone *pivot, Ogre::v1::OldBone *handle
                ) {
			        auto transf = ent->_getParentNodeFullTransform();
                    auto curPos = transf*handle->_getDerivedPosition();

                    Ogre::Ray ray = Ogre::Ray(
                        Ogre::Vector3(curPos.x, this->legAdjustHeight*2, curPos.z),
                        Ogre::Vector3::NEGATIVE_UNIT_Y
                    );
                    auto plane = Ogre::Plane(
                        this->floorNode->getOrientation()*Ogre::Vector3::UNIT_Y,
                        this->floorNode->getPosition()
                    );
                    float dist = Ogre::Math::intersects(ray, plane).second;
                    auto floorPos = ray.getPoint(dist);

                    float factor = curPos.y / this->legAdjustHeight;

                    return floorPos;//(curPos*factor + floorPos*(1.0f-factor));
                };
            }
        }
    }
	void Lab3GameState::destroyScene(void)
    {
        mCharacter->destroy();
        delete mCharacter;
        TutorialGameState::destroyScene();
    }
    //-----------------------------------------------------------------------------------
	void Lab3GameState::update( float timeSinceLast )
	{
        if (selectedObj == Target)
        {
            targetNode->setPosition(targetNode->getPosition()+targetVelocity*timeSinceLast);
        }
        if (selectedObj == Floor)
        {
            floorNormalAndHeight += targetVelocity*timeSinceLast;
            auto realFloorNormal = Ogre::Vector3(floorNormalAndHeight.x, 1, floorNormalAndHeight.z).normalisedCopy();
            auto realFloorPosition = Ogre::Vector3(0, floorNormalAndHeight.y, 0);
            floorNode->setOrientation(Ogre::Vector3::UNIT_Y.getRotationTo(realFloorNormal));
            floorNode->setPosition(realFloorPosition);
        }

        for (auto& name : armAnimNames)
        {
            if (auto ikAnim = dynamic_cast<AnimSystem::IKAnim*>(mCharacter->animController->getAnim(name).get()))
            {
                ikAnim->targetPos = targetNode->getPosition();
            }
        }
        
        mCharacter->update(timeSinceLast);
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
	void Lab3GameState::generateDebugText( float timeSinceLast, Ogre::String &outText )
	{
		TutorialGameState::generateDebugText( timeSinceLast, outText );

        outText += "\nPress R to reload lighting.";
        outText += "\nPress T to move target.";
        outText += "\nPress F to move floor.";
        for (auto& nameKeyPair : mAnimKeys)
        {
            outText += "\nPress '"+std::string(SDL_GetScancodeName(nameKeyPair.second))+"' to play '"+nameKeyPair.first+"'";
        }
        /*outText += "\nPress +/- to move. ";
        outText += "[" + Ogre::StringConverter::toString(splinePos) + "]";*/
	}
    //-----------------------------------------------------------------------------------
	void Lab3GameState::keyPressed( const SDL_KeyboardEvent &arg )
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
        switch (arg.keysym.scancode)
        {
            case SDL_SCANCODE_R:
                loadSceneConfig();
                break;
            case SDL_SCANCODE_F:
                selectedObj = Floor;
                break;
            case SDL_SCANCODE_T:
                selectedObj = Target;
                break;
            case SDL_SCANCODE_LEFT:
                targetVelocity.x = -targetSpeed;
                break;
            case SDL_SCANCODE_RIGHT:
                targetVelocity.x = targetSpeed;
                break;
            case SDL_SCANCODE_UP:
                targetVelocity.z = -targetSpeed;
                break;
            case SDL_SCANCODE_DOWN:
                targetVelocity.z = targetSpeed;
                break;
            case SDL_SCANCODE_KP_PLUS:
                targetVelocity.y = -targetSpeed;
                break;
            case SDL_SCANCODE_KP_MINUS:
                targetVelocity.y = targetSpeed;
                break;
        }
        
        for (auto& nameKeyPair : mAnimKeys)
        {
            if (arg.keysym.scancode == nameKeyPair.second)
            {
                auto anim = mCharacter->animController->getAnim(nameKeyPair.first);
                if (!anim->isEnabled())
                    anim->play();
                else    
                    anim->stop();
                if (auto fkAnim = dynamic_cast<AnimSystem::FKAnim*>(anim.get()))
                {
                    fkAnim->timer->position = 0.0f;
                }
            }
        }

		TutorialGameState::keyPressed( arg );
    }
    //-----------------------------------------------------------------------------------
	void Lab3GameState::keyReleased( const SDL_KeyboardEvent &arg )
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
        switch (arg.keysym.scancode)
        {
            case SDL_SCANCODE_LEFT:
            case SDL_SCANCODE_RIGHT:
                targetVelocity.x = 0;
                break;
            case SDL_SCANCODE_UP:
            case SDL_SCANCODE_DOWN:
                targetVelocity.z = 0;
                break;
            case SDL_SCANCODE_KP_PLUS:
            case SDL_SCANCODE_KP_MINUS:
                targetVelocity.y = 0;
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
