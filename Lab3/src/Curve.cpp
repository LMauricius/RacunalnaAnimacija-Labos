#include "Curve.h"
#include "MeshCreationUtil.h"

#include "OgreRoot.h"

namespace
{
    const Ogre::Matrix4 B = Ogre::Matrix4(
        -1, 3, -3, 1,
        3, -6, 3, 0,
        -3, 0, 3, 0,
        1, 4, 1, 0
    )*(1.0/6.0);

    const Ogre::Matrix4 Bd = Ogre::Matrix4(
        -1, 3, -3, 1,
        2, -4, 2, 0,
        -1, 0, 1, 0,
        0, 0, 0, 0
    )*(1.0/2.0);
}

void Curve::init(Ogre::SceneManager *mgr, const Ogre::String& meshname)
{
    mMgr = mgr;
    mMeshname = meshname;
    
    for (auto p : mPoints) {
        Ogre::Item *item = mMgr->createItem(mMeshname);
        Ogre::SceneNode *node = mMgr->getRootSceneNode()->createChildSceneNode();
        node->attachObject(item);
        node->setPosition(p);
        node->setScale(Ogre::Vector3(0.1));
        node->removeAndDestroyAllChildren();
        mNodes.push_back(node);
    }

    //
    // *** CREATE CURVE MESH ***
    //

    int verticesPerSegment = 20;
    int vertexCount = segmentCount()*verticesPerSegment;

    // Create the mesh:

    std::vector<Ogre::Vector3> verts;
    verts.reserve(vertexCount);
    for(int i=0; i<vertexCount; i++)
    {
        double t = (double)i / verticesPerSegment;
        verts.push_back(getPosition(t));
    }
    Ogre::MeshPtr mesh = createLineMesh("Curve.mesh", verts);

    //
    // *** CREATE THE CURVE ITEM ***
    //
    Ogre::Item *curveItem = mMgr->createItem(mesh);
    mCurveNode = mMgr->getRootSceneNode()->createChildSceneNode();
    mCurveNode->attachObject(curveItem);
}

void Curve::addPoint(Ogre::Vector3 p)
{
    mPoints.push_back(p);
}

void Curve::destroy()
{
    for (auto node : mNodes) {
        node->removeAndDestroyAllChildren();
        mMgr->destroySceneNode(node);
    }
}

Ogre::Vector3 Curve::getPosition(double t)
{
    int segments = mPoints.size() - 3;
    int segInd = std::max(std::min((int)std::floor(t), segments-1), 0)+1;
    double segT = std::fmod(t, 1.0);
    
    return p(segT, segInd).xyz();
}

Ogre::Vector3 Curve::getDerivation(double t)
{
    int segments = mPoints.size() - 3;
    int segInd = std::max(std::min((int)std::floor(t), segments-1), 0)+1;
    double segT = std::fmod(t, 1.0);
    
    return pd(segT, segInd).xyz();
}

Ogre::Vector4 Curve::p(Ogre::Real t, int i)
{
    Ogre::Vector3 p1 = mPoints[i-1];
    Ogre::Vector3 p2 = mPoints[i];
    Ogre::Vector3 p3 = mPoints[i+1];
    Ogre::Vector3 p4 = mPoints[i+2];

    auto res = (
        Ogre::Vector4(t*t*t, t*t, t, 1.0) *
        B *
        Ogre::Matrix4(
            p1.x, p1.y, p1.z, 1.0,
            p2.x, p2.y, p2.z, 1.0,
            p3.x, p3.y, p3.z, 1.0,
            p4.x, p4.y, p4.z, 1.0
        )
    );
    return res;
}

Ogre::Vector4 Curve::pd(Ogre::Real t, int i)
{
    Ogre::Vector3 p1 = mPoints[i-1];
    Ogre::Vector3 p2 = mPoints[i];
    Ogre::Vector3 p3 = mPoints[i+1];
    Ogre::Vector3 p4 = mPoints[i+2];

    auto res = (
        Ogre::Vector4(t*t, t, 1.0, 0.0) *
        Bd *
        Ogre::Matrix4(
            p1.x, p1.y, p1.z, 1.0,
            p2.x, p2.y, p2.z, 1.0,
            p3.x, p3.y, p3.z, 1.0,
            p4.x, p4.y, p4.z, 1.0
        )
    );
    return res;
}