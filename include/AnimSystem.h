#include "ptrs.h"
#include "DeclProperty.h"

#include "OgreEntity.h"

#include <string>
#include <vector>

namespace AnimSystem
{
	// === === === ===
	struct Timer
	{
		bool loop;
		float speed;
		float length;
		float position = 0;

		void update(float timeSinceLast);
	};

	class Anim
	{
	public:
		using property_owner_t = Anim;

		/*decl_property(
			float, weight,
			inline decl_get() {
				return val;
			};
			inline void decl_set(float nval) {
				val = nval;
			}

			private: float val;
		);*/
		Anim(std::list<Anim*> *slot);

		float relativeWeight = 0.0f;
		float maxWeight;
		float fadeInDelay = 0.0f;

		void play(float maxWeight = 1.0f, bool easeIn = true);
		void stop(bool easeOut = true);
		void update(float timeSinceLast);

		void enable();
		void disable();
		inline bool isEnabled() const {return mEnabled;}

		virtual inline void apply(Ogre::v1::Entity *entity, float derivedWeight) {}
		virtual inline void applyLater(Ogre::v1::Entity *entity, float derivedWeight) {}

	protected:
		enum WeightChange {
			Stay,
			EaseIn,
			EaseOut
		} mWeightChange;
		std::list<Anim*> *mSlot;
		bool mEnabled;
	};


	// === === === ===
	class FKAnim : public Anim
	{
	public:
		FKAnim(Timer *timer, std::list<Anim*> *slot, Ogre::String ogreAnimName);
		void apply(Ogre::v1::Entity *entity,float derivedWeight);

		Timer *const timer;

	protected:
		Ogre::String mOgreAnimName;
	};


	// === === === ===
	class IKAnim : public Anim
	{
	public:
		IKAnim(std::list<Anim*> *slot, Ogre::String pivotName, Ogre::String handleName, size_t iterations);
		void applyLater(Ogre::v1::Entity *entity,float derivedWeight);

		Ogre::Vector3 targetPos;
		// entity, pivot, handle
		std::function<Ogre::Vector3(Ogre::v1::Entity *, Ogre::v1::OldBone*, Ogre::v1::OldBone*)> targetAdjustFunction;

	protected:
		Ogre::String mPivotName;
		Ogre::String mHandleName;
		size_t mIterations;
	};


	// === === === ===
	class Controller
	{
	public:
		Controller();

		void init(std::string filename, const std::list<Ogre::v1::Entity*> &entities);
		void update(float timeSinceLast);

		shared<Anim> getAnim(const Ogre::String &name);
		std::list<std::string> getAnimNames() const;

	private:
		std::list<Ogre::v1::Entity *> mEntities;
		std::map<std::string, shared<Anim>> mAnims;
		/*std::map<std::string, shared<FKAnim>> mFKAnims;
		std::map<std::string, shared<IKAnim>> mIKAnims;*/
		std::map<std::string, std::list<Anim*>> mAnimSlots;
		std::map<std::string, Timer> mTimers;
	};
}
