#include "AnimSystem.h"

#include "OgreRoot.h"
//#include "Animation/OgreSkeletonManager.h"
#include "OgreAnimationState.h"
//#include "Animation/OgreSkeletonInstance.h"
#include "OgreOldSkeletonInstance.h"
#include "OgreOldBone.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

#include <math.h>
#include <fstream>

using namespace std;

namespace AnimSystem
{



	// === Anim ===
	void Timer::update(float timeSinceLast)
	{
		position += speed * timeSinceLast;
		if (position >= length)
		{
			if (loop)
			{
				position = fmod(position, length);
			}
			else
			{
				position = length;
			}
		}
	}

	Anim::Anim(std::list<Anim*> *slot):
		mSlot(slot),
		mEnabled(false)
	{
	}

	void Anim::play(float maxWeight, bool easeIn)
	{
		this->maxWeight = maxWeight;
		if (easeIn)
		{
			mWeightChange = EaseIn;
		}

		enable();
	}

	void Anim::stop(bool easeOut)
	{
		if (easeOut)
		{
			mWeightChange = EaseOut;
		}
	}

	void Anim::update(float timeSinceLast)
	{
		switch (mWeightChange)
		{
		case EaseIn:
			relativeWeight += timeSinceLast/fadeInDelay;
			if (relativeWeight >= maxWeight)
			{
				relativeWeight = maxWeight;
				mWeightChange = Stay;
			}
			break;
		case EaseOut:
			relativeWeight -= timeSinceLast/fadeInDelay;
			if (relativeWeight <= 0.0f)
			{
				relativeWeight = 0.0f;
				mWeightChange = Stay;
			}
			break;
		}

		/*if (relativeWeight <= 0.0f)
		{
			disable();
		}*/
	}

	void Anim::enable()
	{
		auto it = find(mSlot->begin(), mSlot->end(), this);
		if (it != mSlot->end())
		{
			mSlot->erase(it);
		}
		else
		{
			relativeWeight = 0.0f;
		}
		mSlot->push_front(this);
		mEnabled = true;
	}

	void Anim::disable()
	{
		/*auto it = find(mSlot->begin(), mSlot->end(), this);
		if (it != mSlot->end())
		{
			mSlot->erase(it);
		}*/
		mEnabled = false;
	}


	// === FK Anim ===
	FKAnim::FKAnim(Timer *timer, std::list<Anim*> *slot, Ogre::String ogreAnimName):
		Anim(slot),
		timer(timer),
		mOgreAnimName(ogreAnimName)
	{
	}

	void FKAnim::apply(Ogre::v1::Entity *entity, float derivedWeight)
	{
		auto animState = entity->getAnimationState(mOgreAnimName);

		if (derivedWeight <= 0.0f)
		{
			//animState->setEnabled(false);
		}
		else
		{
			//animState->setEnabled(true);
			//animState->setTimePosition(timer->position * animState->getLength() / timer->length);
			//animState->setWeight(derivedWeight);

			auto skeleton = entity->getSkeleton();
			auto anim = skeleton->getAnimation(animState->getAnimationName());

			for (auto bIt = skeleton->getBoneIterator(); bIt.hasMoreElements(); bIt.moveNext())
			{
				auto bone = bIt.peekNext();
				size_t boneH = bone->getHandle();

				if (anim->hasOldNodeTrack(boneH))
				{
					auto nodeTrack = anim->getOldNodeTrack(boneH);
					nodeTrack->applyToNode(
						bone,
						timer->position * animState->getLength() / timer->length,
						derivedWeight
					);
				}
			}

			/*auto ntIt = anim->getNodeTrackIterator();
			while (ntIt.hasMoreElements())
			{
				auto nodeTrack = ntIt.getNext();

				nodeTrack->;
			}*/
		}
	}


	// === IK Anim ===
	IKAnim::IKAnim(std::list<Anim*> *slot, Ogre::String pivotName, Ogre::String handleName, size_t iterations):
		Anim(slot),
		mPivotName(pivotName),
		mHandleName(handleName),
		mIterations(iterations)
	{
	}

	namespace
	{
		struct BoneDummy
		{
			Ogre::Vector3 end;
			Ogre::Vector3 start;
			Ogre::Quaternion parentRot;
			Ogre::Vector3 fixedPos;
		};
	}

	void IKAnim::applyLater(Ogre::v1::Entity *entity, float derivedWeight)
	{
		if (derivedWeight <= 0.0f)
		{
		}
		else
		{
			auto skeleton = entity->getSkeleton();
			auto pivot = skeleton->getBone(mPivotName);
			auto handle = skeleton->getBone(mHandleName);

			if (targetAdjustFunction)
			{
				targetPos = targetAdjustFunction(entity, pivot, handle);
			}

			auto transf = entity->_getParentNodeFullTransform();
			auto invTransf = transf.inverse();
			Ogre::Vector3 relTargetPos = invTransf * targetPos;

			for (size_t c = 0; c < mIterations; c++)
			{
				// List bones
				vector<Ogre::v1::OldBone*> realBones;
				vector<BoneDummy> bones;
				Ogre::Quaternion pivotParentRot;

				{
					Ogre::Vector3 pos, parentPos;
					Ogre::Quaternion rot;
					Ogre::Vector3 dummyScale;
					Ogre::v1::OldBone *current = handle;
					do
					{
						realBones.push_back(current);
						//current->_getDerivedPosition().decomposition(pos, dummyScale, rot);
						//current->getParent()->_getDerivedTransform().decomposition(parentPos, dummyScale, rot);
						bones.push_back(BoneDummy{
							current->_getDerivedPosition(),
							current->getParent()->_getDerivedPosition(),
							current->getParent()->getOrientation(),
							current->getPosition()
						});

						current = static_cast<Ogre::v1::OldBone*>(current->getParent());
					} while (current && current != pivot);

					if (pivot->getParent())
					{
						//pivot->getParent()->_getDerivedTransform().decomposition(pos, dummyScale, rot);
						pivotParentRot = pivot->getParent()->_getDerivedOrientation();
					}
					else
					{
						pivotParentRot = Ogre::Quaternion::IDENTITY;
					}
				}

				// FABRIK back algorithm
				Ogre::Vector3 currentTarget = relTargetPos;
				for (int i = 0; i < bones.size(); i++)
				{
					float length = bones[i].end.distance(bones[i].start);
					bones[i].end = currentTarget;

					Ogre::Vector3 boneVec = bones[i].end - bones[i].start;
					boneVec *= length / boneVec.length();
					bones[i].start = bones[i].end - boneVec;

					currentTarget = bones[i].start;
				}

				// FABRIK forward algorithm
				Ogre::Quaternion curRot = pivotParentRot;
				for (int i = bones.size()-1; i >= 0; i--)
				{
					curRot = curRot * bones[i].parentRot;

					auto invRot = curRot.Inverse();

					Ogre::Vector3 boneVecLocal = invRot * (bones[i].end - bones[i].start);
					auto adjustRot = bones[i].fixedPos.getRotationTo(boneVecLocal);
					bones[i].parentRot = bones[i].parentRot * adjustRot;

					curRot = curRot * adjustRot;
				}

				// Save values
				for (int i = 0; i < bones.size(); i++)
				{
					realBones[i]->getParent()->setOrientation(bones[i].parentRot);
				}
			}
		}
	}

	// === Controller ===
	Controller::Controller()
	{

	}


	void Controller::init(std::string filename, const std::list<Ogre::v1::Entity*> &entities)
	{
		mEntities = entities;
		//auto skeleton = mEntities.front()->getSkeletonInstance();

		rapidjson::Document doc;
		ifstream file(filename);
		rapidjson::IStreamWrapper jfile(file);

		// Prepare animations
		for (auto e : mEntities)
		{
			auto animStateIt = e->getAllAnimationStates()->getAnimationStateIterator();
			while (animStateIt.hasMoreElements())
			{
				auto animState = animStateIt.getNext();
				//animState->blend
			}
		}

		// Slots
		doc.ParseStream(jfile);
		for (auto& slotEntry : doc["slots"].GetArray())
		{
			mAnimSlots.insert({slotEntry.GetString(), std::list<Anim*>()});
		}

		// FKAnims
		for (auto& animEntry : doc["FKAnims"].GetObject())
		{
			auto animSpec = animEntry.value.GetObject();

			// ogreAnim
			Ogre::String ogreAnimName = animSpec["ogreAnim"].GetString();
			//auto ogreAnim = skeleton->getAnimation(ogreAnimName);

			// timer
			std::string timerName = animSpec.HasMember("timer")? animSpec["timer"].GetString() : animEntry.name.GetString();
			auto timer = &(mTimers[timerName]);
			timer->loop = animSpec["loop"].GetBool();
			timer->length = mEntities.front()->getAnimationState(ogreAnimName)->getLength();
			timer->speed = animSpec["speed"].GetFloat();

			// slot
			std::string slotName = animSpec.HasMember("slot")? animSpec["slot"].GetString() : animEntry.name.GetString();//animSpec["slot"].GetString();
			list<Anim*> *slot = &(mAnimSlots[slotName]);

			auto anim = make_shared<FKAnim>(FKAnim(timer, slot, ogreAnimName));
			anim->fadeInDelay = animSpec["fadeIn"].GetFloat();

			// insert the anim
			mAnims.insert({animEntry.name.GetString(), anim});
		}

		// IKAnims
		for (auto& animEntry : doc["IKAnims"].GetObject())
		{
			auto animSpec = animEntry.value.GetObject();

			// slot
			std::string slotName = animSpec.HasMember("slot")? animSpec["slot"].GetString() : animEntry.name.GetString();//animSpec["slot"].GetString();
			auto slot = &mAnimSlots[slotName];

			// target, pivot
			std::string pivotName = animSpec["pivotBone"].GetString();
			std::string handleName = animSpec["handleBone"].GetString();
			size_t iters = animSpec["iterations"].GetUint();

			auto anim = make_shared<IKAnim>(slot, pivotName, handleName, iters);

			// insert the anim
			mAnims.insert({animEntry.name.GetString(), anim});
		}
	}

	void Controller::update(float timeSinceLast)
	{
		// Timing
		for (auto& nameTimerPair : mTimers)
		{
			nameTimerPair.second.update(timeSinceLast);
		}

		// Weights
		vector<pair<Anim *, float>> animDerWeights;
		for (auto &nameSlotPair : mAnimSlots)
		{
			float remainingWeight = 1.0f;
			for (auto it = nameSlotPair.second.begin(); it != nameSlotPair.second.end();)
			{
				(*it)->update(timeSinceLast);

				if (remainingWeight * (*it)->relativeWeight > 0.0f)
				{
					animDerWeights.push_back({*it, remainingWeight * (*it)->relativeWeight});
					remainingWeight *= (1.0f - (*it)->relativeWeight);
					it++;
				}
				else
				{
					(*it)->disable();
					nameSlotPair.second.erase(it++);
				}
			}
		}

		// apply
		for (auto e : mEntities)
		{
			auto skeleton = e->getSkeleton();
			for (auto bIt = skeleton->getBoneIterator(); bIt.hasMoreElements(); bIt.moveNext())
			{
				auto bone = bIt.peekNext();
				bone->reset();
				bone->setManuallyControlled(true);
			}

			for (auto& animWeightPair : animDerWeights)
			{
				animWeightPair.first->apply(e, animWeightPair.second);
			}
			for (auto& animWeightPair : animDerWeights)
			{
				animWeightPair.first->applyLater(e, animWeightPair.second);
			}
		}
	}
	

	shared<Anim> Controller::getAnim(const Ogre::String &name)
	{
		auto it = mAnims.find(name);
		if (it != mAnims.end())
		{
			return it->second;
		}
		else
		{
			return nullptr;
		}
	}

	std::list<std::string> Controller::getAnimNames() const
	{
		std::list<std::string> ret;
		for (auto& nameAnimPair : mAnims)
		{
			ret.push_back(nameAnimPair.first);
		}
		return ret;
	}
}