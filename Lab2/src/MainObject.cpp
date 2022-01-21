#include "MainObject.h"

#include "OgreRoot.h"

void MainObject::init(Ogre::SceneManager *mgr, const Ogre::String& meshname)
{
    mMgr = mgr;
    mItem = mMgr->createItem(meshname);
    mNode = mMgr->getRootSceneNode()->createChildSceneNode();
    mNode->attachObject(mItem);
}

void MainObject::destroy()
{
    mNode->removeAndDestroyAllChildren();
    mMgr->destroySceneNode(mNode);
}