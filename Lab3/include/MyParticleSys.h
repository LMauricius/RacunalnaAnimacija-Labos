#include "OgreVector3.h"

#include <list>

class MyParticle
{
public:
    Ogre::Degree rot;
    float life, startLife;
    Ogre::Vector3 velocity;
    Ogre::SceneNode *node;
    Ogre::v1::BillboardSet *billboardSet;
};

class MyParticleSys
{
public:
    MyParticleSys(Ogre::SceneManager *mgr);
    ~MyParticleSys();

    void init();
    void destroy();

    void setMaterial(const Ogre::String& name);
    void setDimensions(float w, float h);
    void setOrigin(const Ogre::Vector3& pos);
    void setParams(
        float loSpawnPerSec, float hiSpawnPerSec,
        float loLife, float hiLife, 
        float loSpeed, float hiSpeed, Ogre::Degree loRot, Ogre::Degree hiRot,
        Ogre::Vector3 dir, Ogre::Degree loDirOffset, Ogre::Degree hiDirOffset,
        bool autoRotate
    );
    void addPlane(const Ogre::Plane& plane);

    void update(float timeSinceLast, Ogre::Camera *cam);

private:
    Ogre::SceneManager *mMgr;
    std::list<MyParticle> mParts;
    std::list<Ogre::Plane> mPlanes;
    bool mInited;

    Ogre::String mMatName;

    Ogre::Vector3 mOrigin;
    float mW, mH;
    float mSpawnTimer, mLoSpawnDelay, mHiSpawnDelay;
    float m_loLife,  m_hiLife;
    float m_loSpeed,  m_hiSpeed;
    Ogre::Degree m_loRot, m_hiRot;
    Ogre::Vector3 m_dir;
    Ogre::Degree m_loDirOffset, m_hiDirOffset;
    bool m_autoRotate;
};