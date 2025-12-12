/*-----------------------------------------------------------------------------
Copyright (c) 2025  Mao-Pao-Tong Workshop

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
-----------------------------------------------------------------------------*/
#pragma once
#include <Ogre.h>
#include <OgreFrameListener.h>
#include <OgreAnimationState.h>
#include <vector>
#include "fg/PathFollow2.h"
#include "fg/Config.h"

namespace fog
{
    using Vector3Ref = Property::Ref<Vector3>;
    /**
     * Move a node to a destination.
     */
    class PathFollow2MissionState : public State, public Stairs
    {
        PathFollow2 path;

        AnimationStateSet *aniSet;

        Vector3 offset;

        float animateTimeSpeedFactor;
        // Vector3Ref actorPosition;
        State *state;
        bool done;

    public:
        PathFollow2MissionState(State *state, PathFollow2 path,                     //
                                AnimationStateSet *aniSet,                          //
                                std::vector<std::string> &aniNames, float aniSpeed, //
                                float heightOffset = 0.0f)                          //
            : state(state),                                                         //
              path(path),                                                           //
              aniSet(aniSet),                                                       //
              animateTimeSpeedFactor(aniSpeed),                                     //
              done(false)                                                           //
        {
            this->aniSet = aniSet;
            this->offset = Vector3(0, heightOffset, 0);
            for (std::string name : aniNames)
            {
                AnimationState *as = this->aniSet->getAnimationState(name);
                as->setEnabled(true);
                as->setLoop(true);
                as->setWeight(1.0f);
            }
            //
            // this->setFrameListener(this);
        }
        void init() override
        {
            // actorPosition = this->getProperty<Vector3>("actor1"
            //                  ".actor.position");
        }

        PathFollow2 &getPath()
        {
            return this->path;
        }

        Vector3 to3D(Point2<float> p){

            return p.transform3(*Context<Transform::D2H2D3>::get()) + this->offset;
        }

        // Vector3 to3D(Vector2 pointIn2D, float scale)
        // {

        //     return Context<Node2D>::get()->to3D(pointIn2D, scale) + this->offset;

        // }

        // bool frameStarted(const Ogre::FrameEvent &evt) override
        bool step(float timeSinceLastFrame)
        {
            if (this->done)
            {
                return false;
            }

            PathFollow2 &pathFollow = this->getPath();

            Vector2 currentPos2D;
            Vector2 direction2D;
            if (!pathFollow.move(timeSinceLastFrame, currentPos2D, direction2D))
            {
                this->done = true;
                std::cout << fmt::format("done") << std::endl;

                int i = 0;
                for (auto it = pathFollow.path.begin(); it != pathFollow.path.end(); it++)
                {
                    std::cout << fmt::format("path[{:>2}]:{:>3.1f},{:>3.1f}", i, (*it).x, (*it).y) << std::endl;
                    
                    i++;
                }
                return false;
            }

            //
            SceneNode *target = this->state->findSceneNode();
            Vector3 prevPos = target->getPosition();

            float terH = 0.0f; // Context<Terrains>::get()->getHeightAtPosition(currentPos2D); // TODO in a common place to translate all .

            //Vector3 currentPos = this->to3D(currentPos2D, Config::CELL_SCALE);//
            Vector3 currentPos = this->to3D(currentPos2D);//

            // position
            Vector3 delta = currentPos - prevPos;

            if (delta.length() > 1000)
            {
                std::cout << fmt::format("current2D:({:>5.1f},{:>5.1f}),delta lenght({}) too big:{:>3.1f},{:>3.1f},{:>3.1f}", currentPos2D.x, currentPos2D.y, delta.length(), delta.x, delta.y, delta.z) << std::endl;
                std::cout << "timeSinceLastFrame:" << timeSinceLastFrame << std::endl;

                int i = 0;
                for (auto it = pathFollow.path.begin(); it != pathFollow.path.end(); it++)
                {
                    std::cout << fmt::format("path[{:>2}]:{:>3.1f},{:>3.1f}", i, (*it).x, (*it).y) << std::endl;
                    i++;
                }
            }

            // delta.y = 0;
            target->translate(delta); // new position
            if (Config::DEBUG_MOVING_POSITION && Config::DEBUG_COUT)
            {
                std::cout << fmt::format("stateMoving: ({:>5.1},{:>5.1},{:>5.1}),({:>5.1},{:>5.1},{:>5.1}),({:>5.1},{:>5.1},{:>5.1})", delta.x, delta.y, delta.z, prevPos.x, prevPos.y, prevPos.z, prevPos.x, prevPos.y, prevPos.z) << std::endl;
            }

            // high

            // animation
            AnimationStateIterator it = this->aniSet->getAnimationStateIterator();
            while (it.hasMoreElements())
            {
                AnimationState *as = it.getNext();
                float aniTimeFactor = this->animateTimeSpeedFactor * (path.getSpeed());
                as->addTime(timeSinceLastFrame * aniTimeFactor);
            }

            Vector3 d3 = Vector3(direction2D.x, 0, -direction2D.y);
            Quaternion orientation = Vector3::UNIT_Z.getRotationTo(d3);

            target->setOrientation(orientation);

            // actorPosition = target->getPosition();
            //  pNode->lookAt();

            // pNode->setOrientation(Quaternion(Degree(90), Vector3::UNIT_Y));
            //   update direction
            //

            return true;
        }
    };
}; // end of namespace
