#include "Character.h"

#include "OgreRoot.h"
#include "OgreSceneManager.h"
#include "OgreResourceManager.h"

#include "Animation/OgreSkeletonInstance.h"
#include "OgreEntity.h"

#include <fstream>
#include <string>

using namespace std;

Character::Character():
	animController(make_unique<AnimSystem::Controller>())
{

}

void Character::init(Ogre::SceneManager *mgr)
{
	mMgr = mgr;
	mMainNode = mMgr->getRootSceneNode()->createChildSceneNode();

	// === Read file for meshes ===

	ifstream file("../../models/druglordParts.txt");
	string part;

	// first line is scale
	getline(file, part);
	mMainNode->setScale(Ogre::StringConverter::parseVector3(part));

	std::list<Ogre::v1::Entity*> mEntities;
	while (getline(file, part))
	{
		auto node = mMainNode->createChildSceneNode();
		auto ent = mMgr->createEntity(part);
		node->attachObject(ent);
		//ent->getAnimationState("Pistol Idle")->setLoop(true);
		//ent->getAnimationState("Pistol Idle")->setEnabled(true);
		//ent->getAnimationState("Pistol Idle")->setTimePosition(0);
		mEntities.push_back(ent);
	}

	animController->init("../../models/druglordAnims.json", mEntities);
}

void Character::destroy()
{
	mMainNode->removeAndDestroyAllChildren();
	mMgr->destroySceneNode(mMainNode);
}


void Character::update(float timeSinceLast)
{
	animController->update(timeSinceLast);
}