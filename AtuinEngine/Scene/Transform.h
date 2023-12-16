
#pragma once


#include "Core/DataStructures/Array.h"
#include "Core/Debug/Log.h"

#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"


namespace Atuin {


class Entity;


class Transform {
    

public:

    Transform() = delete;
    // must be initialized with an associated entity
    explicit Transform( Entity* const entity);
    ~Transform() = default;

    // position
    glm::vec3 Position() const { return mPosition; }
    glm::vec3 WorldPosition();
    void SetPosition( const glm::vec3 &newPosition);
    void SetWorldPosition( const glm::vec3 &newPosition);
    void Translate( const glm::vec3 &deltaPosition);
    void TranslateWorld( const glm::vec3 &deltaPosition);

    // scale
    glm::vec3 Scale() const { return mScale; }
    glm::vec3 WorldScale();
    void SetScale( const glm::vec3 &newScale);
    void SetWorldScale( const glm::vec3 &newScale);

    // rotation
    glm::vec3 Rotation() const { return glm::eulerAngles( mRotation); }
    glm::vec3 WorldRotation();
    glm::quat RotationQuat() const { return mRotation; }
    glm::quat WorldRotationQuat();
    void SetRotation( const glm::vec3 &newRotation);
    void SetWorldRotation( const glm::vec3 &newRotation);
    void SetRotation( const glm::quat &newRotation);
    void SetWorldRotation( const glm::quat &newRotation);
    void Rotate( const glm::vec3 &angles);
    void RotateWorld( const glm::vec3 &angles);
    void Rotate( const glm::vec3 &axis, float angle);
    void RotateWorld( const glm::vec3 &axisWorldSpace, float angle);

    // axes
    glm::vec3 GetLeft();
    glm::vec3 GetUp();
    glm::vec3 GetForward();
    glm::vec3 GetAxis(int i);
    
    // local <---> wold conversions
    const glm::mat4& LocalToWorldMatrix();
    const glm::mat4& WorldToLocalMatrix();
    void SetLocalToWorldMatrix( const glm::mat4 &newMatrix);
    void SetWorldTransform(const  glm::vec3 &newPosition, const glm::vec3 &newEulerAngles, const glm::vec3 &newScale);

    glm::vec3 WorldPositionFromLocalPosition( const glm::vec3 &localPoint);
    glm::vec3 LocalPositionFromWorldPosition( const glm::vec3 &worldPoint);
    glm::vec3 WorldDirectionFromLocalDirection( const glm::vec3 &localDirection);
    glm::vec3 LocalDirectionFromWorldDirection( const glm::vec3 &worldDirection);

    // hierarchy
    void SetParent( Transform* const transform, bool inheritTransform = true);
    Transform* GetParent() const { return pParent; }

    void AddChild(Transform* transform);
    void RemoveChild(Transform* transform);
    void RemoveAllChildren();
    
    Size GetChildCount() const { return pChildren.GetSize(); }
    Array<Transform*>& GetChildren() { return pChildren; }
    Transform* GetChild(U32 childIndex);

    inline std::string GetEntityName() const;

    /// Points to the entity that owns the transform
    Entity* const entity = nullptr;


private:

    void SetDirty();
    void RecalculateLocalToWorldMatrix();

    // local storage
    glm::vec3 mPosition;
    glm::vec3 mScale;
    glm::quat mRotation;

    // cached results
    glm::vec3 mWorldPosition;
    glm::vec3 mWorldScale;
    glm::quat mWorldRotation;
    glm::mat4 mLocalToWorldMatrix;
    glm::mat4 mWorldToLocalMatrix;

    // local space axes
    glm::vec3 mAxis[3];
    glm::vec3& mLeft = mAxis[0];
    glm::vec3& mUp = mAxis[1];
    glm::vec3& mForward = mAxis[2];

    // dirty flags
    bool mDirty = true;
    bool mWorldToLocalDirty = true;

    // parent/child transforms
    Transform* pParent = nullptr;
    Array<Transform*> pChildren;

    Log mLog;
};


} // Atuin
