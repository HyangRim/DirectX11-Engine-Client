#  DirectX11 Engine Portfolio + Eternal Return Copy.

##  프로젝트 소개

> 개인 학습을 기반으로 제작한 DirectX11 기반의 3D 게임 엔진 포트폴리오입니다. + 게임 엔진을 사용해 Eternal Return 모작 프로젝트. 

##  주요 기능

- DirectX11 기반 렌더링 파이프라인 구현. (디퍼드 + 포워드 하이브리드 구성.)
ㄴ Fog Of War -> Shadow -> Deferred(Mesh -> Model -> Animation) -> Outlines -> Transpararent -> Particle -> HUD 순서. 
- 기본 도형 및 모델, 애니메이션 로딩 
ㄴ Assimp를 이용한 FBX 로딩, 내부적인 컨버팅. 
- 모든 가능한 오브젝트 인스턴싱화.
ㄴ Mesh, Material, Texture등 재사용. 드로우 콜 최적화.
- 카메라 및 조명 시스템 (Perspective, Orthogonal), (Ambient, Diffuse, Emissive, Specular)
- 기본적인 셰이더 적용 (Vertex, Pixel, Compute Shader)
- 텍스처 맵핑 및 멀티 텍스처(Manager구조)
ㄴ DiffuseMap, SpecularMap, NormalMap.
- 이벤트 기반 후처리 구조(오브젝트 생성, 삭제등)
- 충돌 관련(Sphere, AABB, OBB) -> 기반 Picking구조.
- ImGui기반 UI(Imguizmo를 통한 Object 배치, Scene 조작)
- TextUI(Direct2D), ImageUI, Button, Slider, ScrollView
- 파티클 시스템(Billboard, SnowBillboard)
- Rigidbody(dynamic, static)
- FOW(전장의 안개)
- 멀티스레드 로딩(리소스, UI 생성)
- 델리게이트 구현. 


## 최적화 관련

- 모델 인스턴싱(드로우 콜 최적화.)
- QuadTree이용한 Frustum Culling
- QuadTree이용한 Collision 최적화, 전장의 안개 최적화. 
- 전장의 안개 시스템을 사용한, GPU 통신과 드로우 콜 최적화. 
- 리소스(Texture, Sound) 로딩, UI 생성을 멀티스레딩을 사용해, 로딩 최적화. 


##  데모 

> 추후 예정. 

##  프로젝트 구조
```
DirectX/
├── AssimpTool/ # FBX(모델, 애니메이션)파일 변환. 
├── Binaries/ # 빌드 파일
├── Client/ # 클라이언트 코드.
├── Engine/ # 엔진 코드.
│ ├── 00. Engine/ # 엔진 필수 자료구조, 인터페이스 
│ ├── 01. Graphics/ # Shader클래스, Buffer클래스등 DirectX 정의. 
│ ├── 02. Managers/ # Game, Graphics, Scene, Input등 매니저.
│ ├── 03. GameObject/ # 게임 내부 Object
│ ├── 04. Component/ # GameObject에 부착할 여러 컴포넌트. 
│ ├── 05. ComputeShader/ # Raw, Structured, Texture Buffer
│ ├── 06. UI & Effect/ # UI(버튼, 텍스트, 이미지, Slider, Scroll View), Billboard등. 
│ ├── 98. Utils/ # Math, ImGui등 외부 헤더 + 사용자 정의 Utils
│ └── 99. Headers # pch, define, Types등. 
├── Libraries/ # Assimp, DirectXTex, Engine lib파일, FMOD, 
├── Resources/ # 이미지, 모델, 오디오 파일 등.  
├── Shaders/ # 쉐이더 코드 파일.  
└── GameCoding.sln # VS 솔루션
```
