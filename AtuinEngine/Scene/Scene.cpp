
#include "Scene.h"
#include "Entity.h"
#include "Transform.h"
#include "Component.h"
#include "Core/Util/StringID.h"


namespace Atuin {


void Scene::Load( const Json &sceneData) {

    pSceneRoot = new Entity( "Root");
    mEntities[ SID("Root")] = pSceneRoot;
   
    for ( auto &[entityName, entityData] : sceneData.GetDict())
    {
        LoadEntity( entityName, entityData);
    }     

    mIsLoaded = true;
}


void Scene::LoadEntity( std::string_view entityName, const Json &entityData) {

    // create new entity
    U64 entityID = SID( entityName.data());
    mEntities[ entityID] = new Entity( entityName);
    Entity *entity = mEntities[ entityID];

    // get parent entity
    U64 parentID = SID( entityData.At("Parent").ToString().c_str());

    // build transform
    entity->transform = new Transform( entity);
    entity->transform->SetParent( mEntities[ parentID]->transform);
    mEntities[ parentID]->transform->AddChild( entity->transform);

    auto &transformData = entityData.At( "Transform");
    auto &position = transformData.At( "Position").GetList();
    auto &rotation = transformData.At( "Rotation").GetList();
    auto &scale = transformData.At( "Scale").GetList();
    entity->transform->SetPosition( glm::vec3{
        position[0].ToFloat(),
        position[1].ToFloat(),
        position[2].ToFloat()
    });
    entity->transform->SetRotation( glm::vec3{
        rotation[0].ToFloat(),
        rotation[1].ToFloat(),
        rotation[2].ToFloat()
    });
    entity->transform->SetScale( glm::vec3{
        scale[0].ToFloat(),
        scale[1].ToFloat(),
        scale[2].ToFloat()
    });

    // build components if any
    if ( entityData.HasKey( "Components"))
    {
        for ( auto &[componentType, componentData] : entityData.At( "Components").GetDict() )
        {
            auto *newComponent = Component::Create( componentType);
            newComponent->Init( componentData);

            entity->AddComponent( newComponent);
        }
    }

    // TODO allow usage of prefabs -> reference other .scene.json file instaead of entity
}


void Scene::Unload() {

    Entity::Destroy( pSceneRoot);
    
    mIsLoaded = false;
}


void Scene::Update() {

    // traverse scene tree, call Entity::Update()
}


void Scene::FixedUpdate() {

    // traverse scenen tree, call Entity::FixedUpdate()
}


void Scene::LateUpdate() {

    // traverse scenen tree, call Entity::LateUpdate()
}


void Scene::CleanUp() {

    // traverse scenen tree, call Entity::LateUpdate()
}


    
} // Atuin
