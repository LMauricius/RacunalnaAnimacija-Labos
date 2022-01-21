#include "OgreString.h"
#include "OgreItem.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"

class MainObject
{
public:
    void init(Ogre::SceneManager *mgr, const Ogre::String& meshname);
    void destroy();

    Ogre::Item *mItem;
    Ogre::SceneManager *mMgr;
    Ogre::SceneNode *mNode;
};