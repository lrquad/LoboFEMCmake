#include "CollisionWorld.h"
#include "LoboMesh/LoboMesh.h"
#include "Collision/CollisionDector/BVHCollisionDetector.h"

Lobo::CollisionWorld::CollisionWorld(/* args */)
{
    all_pairs = true;
}

Lobo::CollisionWorld::~CollisionWorld()
{
}

void Lobo::CollisionWorld::doDetection()
{
    for (int i = 0; i < collisionObjectList.size(); i++)
    {
        collisionObjectList[i]->clearCollitionInfo();
    }

    if (collisionObjectList.size() < 2)
    {
        //no need collision detection
        return;
    }

    if (all_pairs)
    {
        for (int i = 0; i < collisionObjectList.size(); i++)
        {
            for (int j = i + 1; j < collisionObjectList.size(); j++)
            {
                collisionObjectList[i]->collideWith(collisionObjectList[j]);
            }
        }
    }
    else
    {
        for (int i = 0; i < collision_pair.size(); i++)
        {
            int meshA = collision_pair[i].data()[0];
            int meshB = collision_pair[i].data()[1];
            collisionObjectList[meshA]->collideWith(collisionObjectList[meshB]);
        }
    }

    for (int i = 0;i < collisionObjectList.size();i++)
	{
		if (collisionObjectList[i]->getSelfCollisionTest())
		{
			collisionObjectList[i]->selfCollision();
		}
	}
    
}

void Lobo::CollisionWorld::addCollisionDetector(BVHCollisionDetector *col_obj)
{
}

void Lobo::CollisionWorld::addCollisionPair(int idx, int idy)
{
}

void Lobo::CollisionWorld::clearCollisionPair()
{
}
