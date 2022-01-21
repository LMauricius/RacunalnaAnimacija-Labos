#include "AnimSystem.h"
#include "ptrs.h"

#include "OgreString.h"
#include "OgreItem.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"

#include <list>
#include <vector>

class Character
{
public:
    Character(); 
    
    void init(Ogre::SceneManager *mgr);
    void destroy();

    void update(float timeSinceLast);

    std::unique_ptr<AnimSystem::Controller> animController;

protected:
    Ogre::SceneManager *mMgr;
    Ogre::SceneNode *mMainNode;
};