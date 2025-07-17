#include "pch.h"
#include "BillboardDemo.h"
#include "GameObject.h"
#include "GeometryHelper.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Model.h"
#include "ModelRenderer.h"
#include "ModelAnimator.h"
#include "Transform.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Light.h"
#include "TextureBuffer.h"
#include "Viewport.h"
#include "SphereCollider.h"
#include "Scene.h"
#include "AABBBoxCollider.h"
#include "OBBBoxCollider.h"
#include "Terrain.h"
#include "Button.h"
#include "Billboard.h"
#include "SnowBillboard.h"
#include "ParticleSystem.h"
#include "Sky.h"
#include "Rigidbody.h"
#include "FogOfWar.h"
#include "CameraScript.h"
#include "SceneObjectManager.h"
#include "BiancaTest.h"
#include "BiancaCamera.h"

void BillboardDemo::Init()
{
    CURSCENE->SetSky(make_shared<Sky>(L"..\\Resources\\Textures\\Sky\\snowcube1024.dds", L"Sky.fx"));
    shared_ptr<Shader> renderShader = make_shared<Shader>(L"FOW.fx");
    //shared_ptr<Shader> renderShader = make_shared<Shader>(L"23. RenderDemo.fx");
    // Camera
    auto camera = make_shared<GameObject>();
    camera->AddComponent(make_shared<Camera>());
    camera->AddComponent(make_shared<CameraScript>());
    //camera->AddComponent(make_shared<FogOfWar>());

    camera->GetCamera()->SetCullingMaskLayerOnOff(LAYER_UI, true);
    CURSCENE->Add(camera);


    {
        // Mesh0
        // Material
        {
            shared_ptr<Material> material = make_shared<Material>();
            material->SetShader(renderShader);
            auto texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\veigar.jpg");
            material->SetDiffuseMap(texture);
            MaterialDesc& desc = material->GetMaterialDesc();
            desc.ambient = Vec4(1.f);
            desc.diffuse = Vec4(1.f);
            desc.specular = Vec4(1.f);
            RESOURCES->Add(L"Veigar", material);
        }

        //OBB 1
        //{
        //   auto obj = make_shared<GameObject>();
        //   obj->GetTransform()->SetLocalPosition(Vec3(0, 2.5f, 0));
        //   obj->GetTransform()->SetLocalScale(Vec3(1.f));
        //   obj->AddComponent(make_shared<MeshRenderer>());
        //   {
        //      obj->GetMeshRenderer()->SetMaterial(RESOURCES->Get<Material>(L"Veigar"));
        //   }
        //   {
        //      auto mesh = RESOURCES->Get<Mesh>(L"Cube");
        //      obj->GetMeshRenderer()->SetMesh(mesh);
        //      obj->GetMeshRenderer()->SetPass(0);
        //   }
        //   {
        //      //auto collider = make_shared<SphereCollider>();
        //      obj->AddComponent(make_shared<SphereCollider>());
        //      obj->AddComponent(make_shared<Rigidbody>());
        //      obj->GetRigidbody()->SetStatic(true);
        //      //obj->GetButton()->AddOnClikedEvent([obj]() { CURSCENE->Remove(obj); });
        //      
        //   }

        //   CURSCENE->Add(obj);
        //}



        //OBB3
        //{
        //   auto obj = make_shared<GameObject>();
        //   obj->GetTransform()->SetLocalPosition(Vec3(7.5f, 2.5f, 0));
        //   obj->GetTransform()->SetLocalScale(Vec3(1.f));
        //   obj->AddComponent(make_shared<MeshRenderer>());
        //   {
        //      obj->GetMeshRenderer()->SetMaterial(RESOURCES->Get<Material>(L"Veigar"));
        //   }
        //   {
        //      auto mesh = RESOURCES->Get<Mesh>(L"Cube");
        //      obj->GetMeshRenderer()->SetMesh(mesh);
        //      obj->GetMeshRenderer()->SetPass(0);
        //   }
        //   {
        //      auto collider = make_shared<SphereCollider>();
        //      obj->AddComponent(collider);
        //      //obj->GetButton()->AddOnClikedEvent([obj]() { CURSCENE->Remove(obj); });

        //   }
        //   {
        //      obj->AddComponent(make_shared<Rigidbody>());
        //   }
        //   {
        //      obj->AddComponent(make_shared<ForceScript>());
        //   }

        //   CURSCENE->Add(obj);
        //}



    }

    {
        // Light
        auto light = make_shared<GameObject>();
        light->AddComponent(make_shared<Light>());

        LightDesc lightDesc;
        lightDesc.ambient = Vec4(0.4f);
        lightDesc.diffuse = Vec4(1.f);
        lightDesc.specular = Vec4(0.1f);
        lightDesc.direction = Vec3(1.f, 1.f, 1.f);
        //light->GetLight()->SetLightDesc(lightDesc);
        light->GetTransform()->SetRotation(lightDesc.direction);
        light->GetTransform()->SetPosition(Vec3(0.f, 150.f, 0.f));
        static_pointer_cast<Light>(light->GetFixedComponent(ComponentType::Light))->SetLightDesc(lightDesc);
        CURSCENE->Add(light);
    }

    // Billboard
    //{
    //   shared_ptr<Shader> shader = make_shared<Shader>(L"28. BillboardDemo.fx");
    //   auto obj = make_shared<GameObject>();
    //   obj->GetTransform()->SetLocalPosition(Vec3(0.f));
    //   obj->AddComponent(make_shared<Billboard>());
    //   {
    //      // Material
    //      {
    //         shared_ptr<Material> material = make_shared<Material>();
    //         material->SetShader(shader);
    //         auto texture = RESOURCES->Load<Texture>(L"Grass", L"..\\Resources\\Textures\\grass.png");
    //         //auto texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\veigar.jpg");
    //         material->SetDiffuseMap(texture);
    //         MaterialDesc& desc = material->GetMaterialDesc();
    //         desc.ambient = Vec4(1.f);
    //         desc.diffuse = Vec4(1.f);
    //         desc.specular = Vec4(1.f);
    //         RESOURCES->Add(L"Veigar", material);

    //         obj->GetBillboard()->SetMaterial(material);
    //      }
    //   }

    //   for (int32 i = 0; i < 1000; i++)
    //   {
    //      Vec2 scale = Vec2(1 + rand() % 3, 1 + rand() % 3);
    //      Vec2 position = Vec2(-100 + rand() % 200, -100 + rand() % 200);

    //      obj->GetBillboard()->Add(Vec3(position.x, scale.y * 0.5f, position.y), scale);
    //   }

    //   CURSCENE->Add(obj);
    //}

    //// Terrain
    //{
    //   //auto terrainShader = make_shared<Shader>(L"19. RenderDemo.fx");
    //   {
    //      shared_ptr<Material> material = make_shared<Material>();
    //      material->SetShader(renderShader);
    //      auto texture = RESOURCES->Load<Texture>(L"TerrainGrass", L"..\\Resources\\Textures\\Terrain\\grass.jpg");
    //      material->SetDiffuseMap(texture);
    //      MaterialDesc& desc = material->GetMaterialDesc();
    //      desc.ambient = Vec4(1.f);
    //      desc.diffuse = Vec4(1.f);
    //      desc.specular = Vec4(1.f);
    //      RESOURCES->Add(L"TerrainGrass", material);
    //   }
    //   auto obj = make_shared<GameObject>();
    //   obj->SetType(OBJECTTYPE::MAP);
    //   obj->GetTransform()->SetLocalPosition(Vec3(-100.f, 0.f, -100.f));
    //   obj->AddComponent(make_shared<Terrain>());
    //   obj->GetTerrain()->Create(200, 200, RESOURCES->Get<Material>(L"TerrainGrass"));
    //   CURSCENE->Add(obj);
    //}

    //Particle
    /*{
       auto particleShader = make_shared<Shader>(L"ParticleSystem.fx");
       auto obj = make_shared<GameObject>();
       obj->GetTransform()->SetLocalPosition(Vec3(0.f, 5.f, 0.f));
       obj->AddComponent(make_shared<ParticleSystem>());
       shared_ptr<ParticleSystem> particleSystem = obj->GetFixedComponent<ParticleSystem>(ComponentType::ParticleSystem);
       particleSystem->SetEmitDirW(Vec3(0.f, 2.f, 0.f));
       shared_ptr<Material> material = make_shared<Material>();
       material->SetRenderQueue(RenderQueue::Transparent);
       material->SetShader(particleShader);
       auto texture = RESOURCES->Load<Texture>(L"Flare", L"..\\Resources\\Textures\\flare0.png");
       material->SetDiffuseMap(texture);
       material->SetRandomTex(RESOURCES->Get<Texture>(L"RandomTex"));
       particleSystem->SetMaterial(material);
       CURSCENE->Add(obj);
    }*/

    //// SnowBillboard
    //{   // Billboard
    //   {
    //      auto snowShader = make_shared<Shader>(L"29. SnowBillboard.fx");
    //      auto obj = make_shared<GameObject>();
    //      obj->GetTransform()->SetLocalPosition(Vec3(0.f));
    //      obj->AddComponent(make_shared<SnowBillboard>(Vec3(100, 100, 100), 10000));
    //      {
    //         // Material
    //         {
    //            shared_ptr<Material> material = make_shared<Material>();
    //            material->SetShader(snowShader);
    //            auto texture = RESOURCES->Load<Texture>(L"SnowGrass", L"..\\Resources\\Textures\\grass.png");
    //            //auto texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\veigar.jpg");
    //            material->SetDiffuseMap(texture);
    //            MaterialDesc& desc = material->GetMaterialDesc();
    //            desc.ambient = Vec4(1.f);
    //            desc.diffuse = Vec4(1.f);
    //            desc.specular = Vec4(1.f);
    //            RESOURCES->Add(L"Veigar", material);

    //            obj->GetSnowBillboard()->SetMaterial(material);
    //         }
    //      }

    //      CURSCENE->Add(obj);
    //   }
    //}

    {
        // Animation
        shared_ptr<Model> m1 = make_shared<Model>();

        m1->ReadModel(L"Bianca2/Bianca");
        m1->ReadMaterial(L"Bianca2/Bianca");
        m1->ReadAnimation(L"Wait", L"Bianca2/Bianca_wait");
        m1->ReadAnimation(L"Run", L"Bianca2/Bianca_run");
        m1->ReadAnimation(L"BaseAttack", L"Bianca2/Bianca_atk");
        //m1->ReadAnimation(L"Bianca2/Bianca_reststart");
        //m1->ReadAnimation(L"Bianca2/Bianca_restloop");
        //m1->ReadAnimation(L"Bianca2/Bianca_restend");
        m1->ReadAnimation(L"Skill", L"Bianca2/Bianca_dance");

        for (int32 i = 0; i < 1; i++)
        {

            auto obj = make_shared<GameObject>();
            obj->GetTransform()->SetPosition(Vec3(0.f, 0.f, 0.f));
            obj->GetTransform()->SetScale(Vec3(1.f));

            obj->AddComponent(make_shared<ModelAnimator>(renderShader));
            {
                obj->GetModelAnimator()->SetModel(m1);
                obj->GetModelAnimator()->SetPass(2);
            }
            obj->AddComponent(make_shared<AABBBoxCollider>());
            obj->AddComponent(make_shared<FogOfWar>());
            obj->AddComponent(make_shared<BiancaTest>());

            CURSCENE->Add(obj);

            //camera->GetTransform()->SetParent(obj->GetTransform());
            //auto BiancaCam = make_shared<BiancaCamera>();
            //camera->AddComponent(BiancaCam);
            //BiancaCam->SetTarget(obj);
            //BiancaCam->SetOffset(Vec3(0.f, 12.f, -12.5f));
            //camera->GetTransform()->SetRotation(Vec3{ 45.f, 0.f, 0.f });

        }
    }

    {
        // Model
        shared_ptr<Model> m2 = make_shared<Model>();
        m2->ReadModel(L"Cemetary/Cemetary");
        m2->ReadMaterial(L"Cemetary/Cemetary");
        //m2->ReadModel(L"map2/map2");
        //m2->ReadMaterial(L"map2/map2");

        for (int32 i = 0; i < 1; i++)
        {
            auto obj = make_shared<GameObject>();
            obj->GetTransform()->SetPosition(Vec3(0, 0, 0));
            obj->GetTransform()->SetScale(Vec3(0.01f));
            obj->SetType(OBJECTTYPE::MAP);

            obj->AddComponent(make_shared<ModelRenderer>(renderShader));
            {
                obj->GetModelRenderer()->SetModel(m2);
                obj->GetModelRenderer()->SetPass(1);
            }

            CURSCENE->Add(obj);
        }
    }

    // UI
    //{
    //   const int debugUISize = 5;
    //   auto obj = make_shared<GameObject>();
    //   obj->SetLayerIndex(LAYER_UI);
    //   obj->AddComponent(make_shared<Button>());
    //   auto material = make_shared<Material>();
    //   //auto texture = make_shared<Texture>();
    //   //texture->SetSRV(GRAPHICS->GetShadowMapSRV());
    //   material->SetDiffuseMap(GRAPHICS->GetShadowMap());
    //   material->SetShader(make_shared<Shader>(L"DebugTexture.fx"));
    //   obj->GetButton()->Create(Vec2(2048 / (debugUISize * 2) + 20, 2048 / (debugUISize * 2) + 20), Vec2(2048 / debugUISize, 2048 / debugUISize), material);
    //   obj->GetButton()->AddOnClickedEvent([obj]() { CURSCENE->Remove(obj); });

    //   CURSCENE->Add(obj);
    //}

    {
        // UICamera
        auto camera = make_shared<GameObject>();
        camera->GetTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
        camera->AddComponent(make_shared<Camera>());
        camera->GetCamera()->SetProjectionType(ProjectionType::Orthographic);
        camera->GetCamera()->SetNear(1.0f);
        camera->GetCamera()->SetFar(100.0f);
        camera->GetCamera()->SetCullingMaskAll();
        camera->GetCamera()->SetCullingMaskLayerOnOff(LAYER_UI, false);
        CURSCENE->Add(camera);
    }
}

void BillboardDemo::Update()
{
}

void BillboardDemo::Render()
{

}

//버튼이라는 클래스는 OBB Collision을 두고. 
//Ray를 쏘는 방식으로...
//그런데, 우리는 그냥 WINAPI방식으로 한다. Collision방식은 부하가 있음. 
//그림자는 무조건 들어가는 게 좋음. 

//포폴을 만들 때 중요한 건 시간과 노력
//충돌, 레이캐스팅, 애니메이션, 매쉬 로드등과 같은 것. 

void moveScript::Update()
{
    Vec3 pos = GetTransform()->GetPosition();

    pos.x += DT * 1.5f;

    GetTransform()->SetPosition(pos);
}

void ForceScript::Start()
{
    GetRigidbody()->AddForce(Vec3(-8.f, 0.f, 0.f));
}

void ForceScript::Update()
{
    GetRigidbody()->AddForce(Vec3(0.f, 0.f, 0.f));
}


