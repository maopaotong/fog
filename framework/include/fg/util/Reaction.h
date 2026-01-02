#pragma once

#include <vector>
#include <unordered_map>

struct Reaction
{
    using ElementType = uint8_t;

    struct ElementInfo
    {
        ElementType type;
        float quanta;
    };

    struct Node
    {
        ElementInfo *elementInfo;
        float amount;

        Node(float amount, ElementInfo *eleInfo) : amount(amount), elementInfo(eleInfo)
        {
        }
        void offer()
        {
            amount += elementInfo->quanta;
        }

        bool takable()
        {
            return true;
        }

        void take()
        {
            amount -= elementInfo->quanta;
        }
    };

    struct Maker
    {
        std::vector<Node *> sources;
        Node *target;
        bool tryTransform()
        {
            for (Node *it : sources)
            {
                if (!it->takable())
                {
                    return false;
                }
            }

            for (Node *it : sources)
            {
                it->take();
            }
            target->offer();
            return true;
        }
    };
    std::unordered_map<ElementType, ElementInfo> elements; 
    std::vector<Maker *> makers;
    Node timeNode;
    
    Reaction()
    {
        
    }

    void step(float time)
    {
        days += time;
        while (days > 1.0f)
        {
            for (Maker *m : makers)
            {
                m->tryTransform();
            }
            days -= 1.0f;
        }
    }
};
