#include "MeshCreationUtil.h"

#include "OgreRoot.h"
#include "OgreMeshManager2.h"
#include "OgreMesh2.h"
#include "OgreSubMesh2.h"
#include "Vao/OgreVaoManager.h"

Ogre::MeshPtr createLineMesh(Ogre::String meshname, const std::vector<Ogre::Vector3>& positions)
{
    size_t vertexCount = positions.size();

    Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().createManual(meshname,Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::SubMesh* subMesh = mesh->createSubMesh();

    Ogre::RenderSystem* renderSystem = Ogre::Root::getSingleton().getRenderSystem();
    Ogre::VaoManager* vaoManager = renderSystem->getVaoManager();

    Ogre::VertexElement2Vec vertexElements;
    vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_POSITION));
    //vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_NORMAL));
    //    //uvs
    //    for(int i=0; i<currentBuffer->uvSetCount; i++){
    //        vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES));
    //    }

    size_t vertexSize = vaoManager->calculateVertexSize(vertexElements);

    Ogre::Real* vertexData = static_cast<Ogre::Real*>( OGRE_MALLOC_SIMD( vertexSize * vertexCount, Ogre::MEMCATEGORY_GEOMETRY ) );
    Ogre::Real* pVertex = reinterpret_cast<Ogre::Real*>(vertexData);


    Ogre::Vector3 minBB(Ogre::Vector3::UNIT_SCALE*std::numeric_limits<Ogre::Real>::max());
    Ogre::Vector3 maxBB(Ogre::Vector3::UNIT_SCALE*-std::numeric_limits<Ogre::Real>::max());

    for(int i=0; i<vertexCount; i++)
    {
        *pVertex++ = positions[i].x;
        *pVertex++ = positions[i].y;
        *pVertex++ = positions[i].z;

        /*Ogre::Vector3 norm = Ogre::Vector3::UNIT_Y;//convertToYup(Ogre::Vector3(currentBuffer->vertexs.at(i).no[0],currentBuffer->vertexs.at(i).no[1],currentBuffer->vertexs.at(i).no[2])).normalisedCopy();
        //Normals
        *pVertex++ = norm.x;
        *pVertex++ = norm.y;
        *pVertex++ = norm.z;*/

        //        //uvs
        //        for(int j=0; j<currentBuffer->uvSetCount; j++){
        //            *pVertex++ = currentBuffer->vertexs.at(i).uv[j].x;
        //            *pVertex++ = 1.0-currentBuffer->vertexs.at(i).uv[j].y;
        //        }

        //Calc Bounds
        minBB.makeFloor(positions[i]);
        maxBB.makeCeil(positions[i]);

    }

    Ogre::VertexBufferPackedVec vertexBuffers;

    Ogre::VertexBufferPacked *pVertexBuffer = vaoManager->createVertexBuffer( vertexElements, vertexCount, Ogre::BT_IMMUTABLE, vertexData, true );
    vertexBuffers.push_back(pVertexBuffer);



    //Indices

    //unsigned int iBufSize = currentBuffer->triangles.size() * 3;
    unsigned int indexCount = vertexCount;

    static const unsigned short index16BitClamp = (0xFFFF) - 1;

    //Index buffer
    Ogre::IndexBufferPacked::IndexType buff_type = (vertexCount > index16BitClamp) ?
                Ogre::IndexBufferPacked::IT_32BIT : Ogre::IndexBufferPacked::IT_16BIT;

    //Build index items
    bool using32 = (buff_type == Ogre::IndexBufferPacked::IT_32BIT);

    Ogre::uint32 *indices32 = 0;
    Ogre::uint16 *indices16 = 0;

    if (!using32)
        indices16 = reinterpret_cast<Ogre::uint16*>( OGRE_MALLOC_SIMD(sizeof(Ogre::uint16) * indexCount, Ogre::MEMCATEGORY_GEOMETRY ) );
    else
        indices32 = reinterpret_cast<Ogre::uint32*>( OGRE_MALLOC_SIMD(sizeof(Ogre::uint32) * indexCount, Ogre::MEMCATEGORY_GEOMETRY ) );

    for (unsigned int i = 0; i < indexCount; i++)
    {
        if(using32)
            indices32[i] = (Ogre::uint32)i;
        else
            indices16[i] = (Ogre::uint16)i;
    }

    Ogre::IndexBufferPacked *indexBuffer;
    if(using32){
        indexBuffer = vaoManager->createIndexBuffer( buff_type, indexCount, Ogre::BT_IMMUTABLE, indices32, true );
    }
    else{
        indexBuffer = vaoManager->createIndexBuffer( buff_type, indexCount, Ogre::BT_IMMUTABLE, indices16, true );
    }


    Ogre::VertexArrayObject *vao = vaoManager->createVertexArrayObject(
                vertexBuffers, indexBuffer, Ogre::OperationType::OT_LINE_STRIP );

    subMesh->mVao[0].push_back( vao );
    subMesh->mVao[1].push_back( vao );


    Ogre::Aabb bounds;
    bounds.merge(minBB);
    bounds.merge(maxBB);
    mesh->_setBounds(bounds,false);
    mesh->_setBoundingSphereRadius(bounds.getRadius());

    return mesh;
}