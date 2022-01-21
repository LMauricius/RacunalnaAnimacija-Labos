#include "OgreString.h"
#include "OgreItem.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"

#include <list>
#include <vector>

class Curve
{
public:
    void init(Ogre::SceneManager *mgr, const Ogre::String& meshname);
    void addPoint(Ogre::Vector3 p);
    Ogre::Vector3 getPosition(double t);
    Ogre::Vector3 getDerivation(double t);
    void destroy();

    inline size_t pointCount() const {return mPoints.size();}
    inline size_t segmentCount() const {return mPoints.size() - 3;}

protected:
    Ogre::String mMeshname;
    Ogre::SceneManager *mMgr;
    Ogre::SceneNode *mCurveNode;
    std::list<Ogre::SceneNode*> mNodes;
    std::vector<Ogre::Vector3> mPoints;

    Ogre::Vector4 p(Ogre::Real t, int i);
    Ogre::Vector4 pd(Ogre::Real t, int i);
};