
#pragma once


#include "Core/Util/Types.h"

#include <bitset>


namespace Atuin {


class Entity;


class Component {


protected:  

    enum class ComponentState : U8 {
        ACTIVE,
        AWAKE,
        DESTROY,
        UPDATE,
        FIXED_UPDATE,
        LATE_UPDATE,
        NUM_STATES
    };


public:

    virtual ~Component() = default;

    bool IsActive() const;
    void SetActive( bool value);

    bool GetState( ComponentState state) const;
    void SetState( ComponentState state, bool value);

    virtual void OnEnable() = 0;
    virtual void Awake() = 0;
    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void FixedUpdate() = 0;
    virtual void LateUpdate() = 0;
    virtual void OnDisable() = 0;
    virtual void OnDestroy() = 0;


    Entity* const entity;


protected:

    Component();

    std::bitset<(Size)ComponentState::NUM_STATES> mState;
};

    
} // Atuin
