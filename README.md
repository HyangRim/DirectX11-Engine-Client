#  DirectX11 Engine Portfolio

> 개인 학습을 기반으로 제작한 DirectX11 기반의 3D 게임 엔진 포트폴리오입니다.

##  프로젝트 소개

이 프로젝트는 DirectX11 강의를 기반으로 개발된 3D 게임 엔진입니다.
[https://www.inflearn.com/course/directx11-%EA%B2%8C%EC%9E%84%EA%B0%9C%EB%B0%9C-%EB%8F%84%EC%95%BD%EB%B0%98]

##  주요 기능

- DirectX11 기반 렌더링 파이프라인 구현. 
- 기본 도형 및 모델, 애니메이션 로딩
- 모든 가능한 오브젝트 인스턴스화.
- 카메라 및 조명 시스템 (Perspective, Orthogonal), (Ambient, Diffuse, Emissive, Specular)
- 기본적인 셰이더 적용 (Vertex, Pixel, Compute Shader)
- 텍스처 맵핑 및 멀티 텍스처(Manager구조)
- 간단한 씬 그래프 구조
- 충돌 관련(Sphere, AABB, OBB)
- ImGui기반 UI


### 예정 기능

- 파티클 시스템 (제작 중.)
- Shadow
- Rigidbody
- 멀티스레드 로딩. 

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
│ ├── 06. UI & Effect/ # UI(버튼), Billboard등. 
│ ├── 98. Utils/ # Math, ImGui등 외부 헤더 + 사용자 정의 Utils
│ └── 99. Headers # pch, define, Types등. 
├── Libraries/ # Assimp, DirectXTex, Engine lib파일, DirectX11
├── Resources/ # 이미지, 모델, 오디오 파일 등.  
├── Shaders/ # 쉐이더 코드 파일.  
└── GameCoding.sln # VS 솔루션
```
