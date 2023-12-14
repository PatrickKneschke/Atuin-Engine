
#include "Transform.h"
#include "Entity.h"
#include "Core/Util/StringFormat.h"


namespace Atuin {


Transform::Transform( Entity* const entity_) : entity { entity_} {}
    

glm::vec3 Transform::WorldPosition() {

    return LocalToWorldMatrix()[3];
}


void Transform::SetPosition( const glm::vec3 &newPosition) {

    SetDirty();

    mPosition = newPosition;
}


void Transform::SetWorldPosition( const glm::vec3 &newPosition) {

    SetDirty();

    if ( pParent == nullptr) 
    {
        mPosition = newPosition;
    } 
    else 
    {
        mPosition = (pParent->WorldToLocalMatrix() * glm::vec4{ newPosition, 1});
    }
}


void Transform::Translate( const glm::vec3 &deltaPosition) {

    SetPosition( mPosition + deltaPosition);
}


void Transform::TranslateWorld( const glm::vec3 &deltaPosition) {

    SetWorldPosition( WorldPosition() + deltaPosition);
}


glm::vec3 Transform::WorldScale() {

    if ( pParent == nullptr) 
    {
        mWorldScale = mScale;
    } 
    else 
    {
        mWorldScale = pParent->WorldScale() * mScale;
    }

    return mWorldScale;
}


void Transform::SetScale( const glm::vec3 &newScale) {

    SetDirty();

    mScale = newScale;
}


void Transform::SetWorldScale( const glm::vec3 &newScale) {

    SetDirty();

    if ( pParent == nullptr)
    {
        mScale = newScale;
    }
    else 
    {
        mScale = newScale / pParent->WorldScale();
    }
}


glm::vec3 Transform::WorldRotation() {

    return glm::eulerAngles( WorldRotationQuat());
}


glm::quat Transform::WorldRotationQuat() {

    if ( pParent == nullptr) 
    {
        mWorldRotation = mRotation;
    } 
    else 
    {
        mWorldRotation = pParent->WorldRotationQuat() * mRotation;
    }

    return mWorldRotation;
}


void Transform::SetRotation( const glm::vec3 &newRotation) {

    SetRotation( glm::quat( newRotation));
}


void Transform::SetWorldRotation( const glm::vec3 &newRotation) {

    SetWorldRotation( newRotation);
}


void Transform::SetRotation( const glm::quat &newRotation) {

    SetDirty();

    mRotation = newRotation;
}


void Transform::SetWorldRotation( const glm::quat &newRotation) {

    SetDirty();

    mWorldRotation = newRotation;

    if ( pParent == nullptr) 
    {
        mRotation = mWorldRotation;
    } 
    else 
    {
        mRotation = glm::inverse( pParent->WorldRotationQuat()) * mWorldRotation;
    }
}


void Transform::Rotate( const glm::vec3 &angles) {

    glm::vec3 left, up, forward;
    // TODO move the parent check into axis getter functions ?
    if ( pParent != nullptr)
    {
        left = pParent->LocalDirectionFromWorldDirection( GetLeft());
        up = pParent->LocalDirectionFromWorldDirection( GetUp());
        forward = pParent->LocalDirectionFromWorldDirection( GetForward());
    } 
    else 
    {
        left = GetLeft();
        up = GetUp();
        forward = GetForward();
    }
    
    SetRotation( glm::quat(left * angles.x + up * angles.y + forward * angles.z) * mRotation);
}


void Transform::RotateWorld( const glm::vec3 &angles) {

    SetWorldRotation( glm::quat( angles) * WorldRotation());
}


void Transform::Rotate( const glm::vec3 &worldAxis, float angle) {

    glm::vec3 localAxis = pParent != nullptr ? pParent->LocalDirectionFromWorldDirection( worldAxis) : worldAxis;
    SetRotation( glm::quat( angle, localAxis) * mRotation);
}


void Transform::RotateWorld( const glm::vec3& axisWorldSpace, float angle) {

    SetWorldRotation( glm::quat(angle, axisWorldSpace));
}


glm::vec3 Transform::GetLeft() {

    return GetAxis( 0);
}


glm::vec3 Transform::GetUp() {

    return GetAxis( 1);
}


glm::vec3 Transform::GetForward() {

    return GetAxis( 2);
}


glm::vec3 Transform::GetAxis(int i) {

    LocalToWorldMatrix();

    return mAxis[i];    
}


const glm::mat4& Transform::LocalToWorldMatrix() {

    if ( mDirty) 
    {
        RecalculateLocalToWorldMatrix();
        mDirty = false;
    }

    return mLocalToWorldMatrix;
}


const glm::mat4& Transform::WorldToLocalMatrix() {

    if ( mWorldToLocalDirty) 
    {
        mWorldToLocalMatrix = glm::inverse( LocalToWorldMatrix());
        mWorldToLocalDirty = false;
    }

    return mWorldToLocalMatrix;
}


void Transform::SetLocalToWorldMatrix( const glm::mat4 &newMatrix) {

    SetDirty();

    mLocalToWorldMatrix = newMatrix;
    for ( int i = 0; i <= 2; i++) 
    {
        mAxis[i] = glm::normalize( mLocalToWorldMatrix[i]);
    }
}


void Transform::SetWorldTransform(const  glm::vec3 &newPosition, const glm::vec3 &newAngles, const glm::vec3 &newScale) {

    SetWorldPosition( newPosition);
    SetWorldRotation( newAngles);
    SetWorldScale( newScale);
}


glm::vec3 Transform::WorldPositionFromLocalPosition( const glm::vec3 &localPoint) {

    return LocalToWorldMatrix() * glm::vec4( localPoint, 1);
}


glm::vec3 Transform::LocalPositionFromWorldPosition( const glm::vec3 &worldPoint) {

    return WorldToLocalMatrix() * glm::vec4( worldPoint, 1);
}


glm::vec3 Transform::WorldDirectionFromLocalDirection( const glm::vec3 &localDirection) {

    return LocalToWorldMatrix() * glm::vec4( localDirection, 0);
}


glm::vec3 Transform::LocalDirectionFromWorldDirection( const glm::vec3 &worldDirection) {

    return WorldToLocalMatrix() * glm::vec4( worldDirection, 0);    
}


void Transform::SetParent( Transform* const transform, bool inheritTransform) {

    Transform* parentTransform = transform;
    if (transform == nullptr) 
    {
        // get root transform from scene
    }
    if ( pParent == parentTransform) 
    {
        mLog.Warning( LogChannel::GRAPHICS ,FormatStr( "Trying to set parent of %s to %s, which is already it's parent.", GetEntityName(), parentTransform->GetEntityName()));
        return;
    }

    glm::vec3 originalPosition, originalScale;
    glm::quat originalRotation;
    if ( !inheritTransform) 
    {
        originalPosition = WorldPosition();
        originalRotation = WorldRotationQuat();
        originalScale = WorldScale();
    }

    if ( pParent != nullptr) 
    {
        pParent->RemoveChild(this);
    }
    if ( parentTransform != nullptr) 
    {
        parentTransform->AddChild(this);
    }

    pParent = parentTransform;
    if ( !inheritTransform) 
    {
        SetWorldPosition( originalPosition);
        SetWorldRotation( originalRotation);
        SetWorldScale( originalScale);
    }

    SetDirty();
}


void Transform::AddChild(Transform* transform) {

    pChildren.PushBack( transform);
}


void Transform::RemoveChild(Transform* transform) {

    for ( Size i = 0; i < pChildren.GetSize(); i++) 
    {
        if ( pChildren[i] == transform) 
        {
            pChildren.Erase( i);

            return;
        }
    }
}


Transform* Transform::GetChild(U32 childIndex) {

    if ( childIndex > GetChildCount())
    {
        mLog.Error( LogChannel::GRAPHICS, FormatStr( "Tried to acces child index %d, which is out of range of [0, %d].", childIndex, GetChildCount()));
    }

    return pChildren[ childIndex];
}


void Transform::SetDirty() {

    mDirty = true;
    mWorldToLocalDirty = true;
    
    // TODO set children dirty ?
}


void Transform::RecalculateLocalToWorldMatrix() {

    glm::mat4 localToParentMatrix = glm::scale( glm::mat4(1), Scale());
    localToParentMatrix = glm::rotate( localToParentMatrix, glm::angle( mRotation), glm::axis( mRotation));
    localToParentMatrix = glm::translate( localToParentMatrix, mPosition);

    if ( pParent != nullptr) 
    {
        mLocalToWorldMatrix = pParent->LocalToWorldMatrix() * localToParentMatrix;
    } 
    else 
    {
        mLocalToWorldMatrix = localToParentMatrix;
    }

    for (int i = 0; i <= 2; i++) 
    {
        mAxis[i] = glm::normalize( glm::vec3( mLocalToWorldMatrix[i]));
    }
}


std::string Transform::GetEntityName() const { 
    
    return entity->Name(); 
}



} // Atuin
