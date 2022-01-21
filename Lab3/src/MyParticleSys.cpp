#include "MyParticleSys.h"

#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OgreBillboardSet.h"
#include "OgreBillboard.h"

#include <random>
#include <ctime>

namespace {
    float randRange(float lo, float hi)
    {
        return (lo + (float)rand()/RAND_MAX * (hi - lo));
    }
}

MyParticleSys::MyParticleSys(Ogre::SceneManager *mgr):
    mInited(false),
    mMgr(mgr)
{

}

MyParticleSys::~MyParticleSys()
{
    if (mInited)
    {
        destroy();
    }
}

void MyParticleSys::init()
{
    mInited = true;

    //mBillboards = mMgr->createBillboardSet();
}

void MyParticleSys::destroy()
{
    mInited = false;

    for (auto p : mParts)
    {
        //p.node->removeAndDestroyAllChildren();
        p.node->getParentSceneNode()->removeAndDestroyChild(p.node);
    }

}

void MyParticleSys::setMaterial(const Ogre::String& name)
{
    mMatName = name;
}

void MyParticleSys::setDimensions(float w, float h)
{
    mW=w;
    mH=h;
}

void MyParticleSys::setOrigin(const Ogre::Vector3& pos)
{
    mOrigin = pos;
}

void MyParticleSys::setParams(
    float loSpawnPerSec, float hiSpawnPerSec,
    float loLife, float hiLife, 
    float loSpeed, float hiSpeed, Ogre::Degree loRot, Ogre::Degree hiRot,
    Ogre::Vector3 dir, Ogre::Degree loDirOffset, Ogre::Degree hiDirOffset,
    bool autoRotate
)
{
    mLoSpawnDelay = 1.0/hiSpawnPerSec;
    mHiSpawnDelay = 1.0/loSpawnPerSec;
    mSpawnTimer = 0.0;
    m_loLife = loLife;
    m_hiLife = hiLife;
    m_loSpeed = loSpeed;
    m_hiSpeed = hiSpeed;
    m_loRot = loRot;
    m_hiRot = hiRot;
    m_dir = dir.normalisedCopy();
    m_loDirOffset = loDirOffset;
    m_hiDirOffset = hiDirOffset;
    m_autoRotate = autoRotate;
}

void MyParticleSys::addPlane(const Ogre::Plane& plane)
{
    mPlanes.push_back(plane);
}

void MyParticleSys::update(float timeSinceLast, Ogre::Camera *cam)
{
    mSpawnTimer -= timeSinceLast;
    while (mSpawnTimer <= 0.0)
    {
        mSpawnTimer += randRange(mLoSpawnDelay, mHiSpawnDelay);
        m_dir.perpendicular();
        mParts.push_back(MyParticle{
            .rot = Ogre::Degree(randRange(m_loRot.valueDegrees(), m_hiRot.valueDegrees())),
            .life = randRange(m_loLife, m_hiLife),
            .startLife=0,
            .velocity = (
                (Ogre::Quaternion(Ogre::Degree(randRange(0, 360)), m_dir)*
                Ogre::Quaternion(Ogre::Degree(randRange(m_loDirOffset.valueDegrees(), m_hiDirOffset.valueDegrees())), m_dir.perpendicular()))*
                m_dir*randRange(m_loSpeed, m_hiSpeed)
            ),
            .node = mMgr->getRootSceneNode()->createChildSceneNode(),
            .billboardSet = mMgr->createBillboardSet()
        });

        mParts.back().startLife = mParts.back().life;
        mParts.back().node->setPosition(mOrigin);
        mParts.back().billboardSet->setDatablockOrMaterialName(mMatName, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
        auto board = mParts.back().billboardSet->createBillboard(Ogre::Vector3::ZERO);
        board->setDimensions(mW, mH);
        //mParts.back().billboardSet->endBillboards();
        mParts.back().node->attachObject(mParts.back().billboardSet);
    }

    for (auto it = mParts.begin(); it != mParts.end(); )
    {
        it->life -= timeSinceLast;
        if (it->life <= 0.0)
        {
            it->node->getParentSceneNode()->removeAndDestroyChild(it->node);
            //mMgr->destroyBillboardSet(it->billboardSet);
            mParts.erase(it++);
        }
        else
        {
            auto& n = it->node;
            n->setPosition(n->getPosition() + it->velocity*timeSinceLast);

            for (auto& pl : mPlanes)
            {
                if (
                    pl.getSide(n->getPosition()) == Ogre::Plane::Side::NEGATIVE_SIDE &&
                    pl.normal.dotProduct(it->velocity) < 0.0
                )
                {
                    it->velocity = it->velocity - 2*it->velocity.dotProduct(pl.normal)*pl.normal;
                }
            }

            if (m_autoRotate)
            {

            }

            ++it;
        }
    }
}