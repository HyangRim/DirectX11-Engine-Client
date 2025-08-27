# DirectX11 Engine Portfolio + *Eternal Return* 모작

## 프로젝트 소개
- 개인 학습을 기반으로 제작한 **DirectX11 기반 3D 게임 엔진 포트폴리오**입니다.
- 현재 **Eternal Return**을 모티브로, 해당 게임의 핵심 시스템을 재현하는 모작 프로젝트를 진행 중입니다.
- GitHub 저장소 링크: [HyangRim/DirectX11-Engine-Client](https://github.com/HyangRim/DirectX11-Engine-Client)

## 주요 기능

### 렌더링 파이프라인
1. **Fog of War**
2. **Shadow**
3. **Deferred** (Mesh → Model → Animation)
4. **Outlines**
5. **Transparent**
6. **Particle**
7. **HUD**

하이브리드 구조 (디퍼드 + 포워드 구성)

### 리소스 로딩 & 관리
- Assimp 기반 FBX 모델 및 애니메이션 로딩 (내부 컨버팅 포함)
- Mesh, Material, Texture 등의 오브젝트 인스턴싱을 통한 드로우 콜 최적화
- 텍스처 매핑 및 멀티 텍스처 시스템 (DiffuseMap, SpecularMap, NormalMap – Manager 구조)

### 셰이더, UI 및 인터랙션
- Vertex, Pixel, Compute 셰이더 지원
- 이벤트 기반 후처리 시스템 (오브젝트 생성/삭제 등)
- UI 시스템:
  - ImGui 기반 인터페이스
  - ImGuizmo 활용 오브젝트 배치 & 씬 조작
  - Direct2D 기반 UI 요소: TextUI, ImageUI, Button, Slider, ScrollView

### 물리 및 충돌
- 충돌 처리 및 Picking: Sphere, AABB, OBB 기반 충돌
- 파티클 시스템: Billboard, SnowBillboard
- Rigidbody 시스템: Dynamic & Static 구성

### 멀티스레딩 & 최적화
- 멀티스레드 기반 리소스 및 UI 로딩
- 델리게이트 구현
- 최적화 기술:
  - QuadTree 기반 Frustum Culling
  - QuadTree 기반 충돌 및 Fog of War 최적화
  - GPU 통신 최적화 및 드로우 콜 최소화

## 프로젝트 구조


```
DirectX/
├── AssimpTool/ # FBX (모델·애니메이션) 변환 도구
├── Binaries/ # 빌드 생성 파일
├── Client/ # 클라이언트 실행 코드
├── Engine/ # 엔진 핵심 코드
│ ├── 00. Engine/ # 기본 자료구조, 인터페이스
│ ├── 01. Graphics/ # Shader, Buffer 등 DirectX 정의
│ ├── 02. Managers/ # Game, Graphics, Scene, Input 등 매니저
│ ├── 03. GameObject/ # 게임 오브젝트
│ ├── 04. Component/ # 컴포넌트 기반 구조
│ ├── 05. ComputeShader/ # Raw, Structured, Texture Buffer 처리
│ ├── 06. UI & Effect/ # UI 요소 및 이펙트 (버튼, 텍스트, 이미지, Slider, ScrollView, Billboard 등)
│ ├── 98. Utils/ # 수학, ImGui 등 외부 + 사용자 정의 유틸
│ └── 99. Headers/ # pch, define, Types 등 공통 헤더
├── Libraries/ # Assimp, DirectXTex, Engine lib, FMOD 등
├── Resources/ # 이미지, 모델, 오디오 파일
├── Shaders/ # HLSL Shader 코드
└── GameCoding.sln # Visual Studio 솔루션 파일
```
