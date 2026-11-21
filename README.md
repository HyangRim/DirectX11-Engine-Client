# DirectX 11 Engine — *Eternal Return* 모작

DirectX 11 / C++로 만든 3D 엔진 위에서 **Eternal Return**의 핵심 플레이를 재현한 2인 팀 프로젝트입니다.

![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=cplusplus)
![DirectX 11](https://img.shields.io/badge/DirectX-11-107C10)
![Visual Studio 2022](https://img.shields.io/badge/Visual%20Studio-2022%20(v143)-5C2D91?logo=visualstudio)
![Platform](https://img.shields.io/badge/Platform-Windows%20x64-0078D6?logo=windows)

| 기간 | 인원 | 개발 환경 |
|---|---|---|
| 2025.06 ~ 2026.01 (팀 프로젝트)<br>2026.11 (개인 개선 작업) | 2인 | C++17 · DirectX 11 · HLSL · Visual Studio 2022 |

---

## 역할 분담

| | 담당 |
|---|---|
| **팀원** | 엔진 베이스 (강의 기반) — 렌더링 파이프라인, 셰이더, 리소스 로딩, UI 프레임워크 |
| **본인** | 경로 탐색 (NavMesh · A\*), 오브젝트 관리, 캐릭터 · 몬스터 상태 머신과 스킬, 아이템 · 인벤토리, QuadTree 기반 Picking |
| **본인 (2026.11)** | NavMesh 경로 스무딩 개선, 충돌 처리 Loose QuadTree 재설계 |

---

## 개선 작업 (2026.11)

### 1. NavMesh 경로 스무딩 — 코너 끼임 해결

**문제** — 기존 LOS(시야 검사) 방식 스무딩은 벽 코너를 깎아 지나가면서 캐릭터가 코너에 끼었고, 스무딩 한 번에 수십 ms가 걸렸습니다.

**해결** — A\*가 지나간 삼각형 통로(Corridor) 안에서만 경로를 펴는 방식으로 바꾸고, Funnel 알고리즘과 비교했습니다.

| 무작위 1,000쌍 (Debug) | 기존 LOS | Corridor | Funnel |
|---|---|---|---|
| 도착률 | 90.9% | 99.9%¹ | 100% |
| 코너에 끼인 경로 | 93 | **0** | **0** |
| 벽을 침범한 경로 | 569 | **0** | **0** |
| 스무딩 시간 (중앙값) | 17,736 μs | **11.2 μs** | 12.8 μs |

¹ 미도착 1쌍은 끼임이 아니라 30초 제한 시간 초과

### 2. 충돌 처리 — 화면 QuadTree → 월드 Loose QuadTree

**문제** — 기존 충돌은 카메라 **화면 좌표** QuadTree에 **화면 안 객체만** 넣어 검사했습니다.
화면 밖 충돌을 놓쳤고, 카메라가 움직일 때마다 트리를 다시 지었으며, 몬스터가 한곳에 몰리면 트리가 깊이 12까지 쪼개져 게임이 멈췄습니다.

**해결** — 월드 XZ 좌표 기준 **Loose QuadTree**로 재설계했습니다. 객체를 크기에 맞는 레벨의 칸 하나에만 넣고, 칸 경계를 사방으로 절반씩 넓혀 다룹니다.

| 늑대 200마리 (Release) | 기존 | Loose QuadTree |
|---|---|---|
| 놓친 충돌 쌍 (브루트포스 정답 대비) | 79% | **0%** |
| 흩어진 장면 · 충돌 처리 | 0.177 ms | **0.061 ms** |
| 몰린 장면 · 충돌 처리 | 247 ms | **0.26 ms** |
| 몰린 장면 · 프레임 전체 | 536 ms | **8.2 ms** |

### 3. Release 빌드에서만 멈추던 버그

`assert(::ReadFile(...))`, `#define CHECK(p) assert(SUCCEEDED(p))`처럼 **assert 안에서 함수를 호출**하던 코드가 Release(`NDEBUG`)에서 호출째 사라져, 모델 파일을 읽지 않은 채 쓰레기 길이로 할당을 반복하고 있었습니다.
호출은 항상 하고 결과만 검사하도록 바꿔 53곳을 한 번에 고쳤습니다.

---

## 주요 기능

**게임 플레이**
- 플레이어블 캐릭터 2종 (Nicky, Bianca) — 기본 공격, Q / W / E / R 스킬, 카운터
- 야생 동물 (Wolf, Alpha) — 상태 머신 · 행동 트리 AI
- 아이템 상자, 인벤토리, 장비, 제작
- Fog of War, HUD, 캐릭터 선택 화면

**엔진**
- 렌더링 — Deferred + Forward 하이브리드, Shadow, Outline, Transparent, Particle
- 리소스 — Assimp 기반 FBX 모델 · 애니메이션 변환 (AssimpTool), 인스턴싱
- 셰이더 — Vertex / Pixel / Compute Shader
- UI — ImGui · ImGuizmo 에디터, Direct2D UI (Text, Image, Button, Slider, ScrollView)
- 공간 분할 — QuadTree Frustum Culling · Picking, Loose QuadTree 충돌
- 기타 — 멀티스레드 리소스 로딩, 이벤트 기반 오브젝트 생성 / 삭제, 델리게이트

---

## 빌드

1. Visual Studio 2022 (v143 툴셋)로 `GameCoding.sln`을 엽니다.
2. 구성을 `x64`로 설정합니다.
3. **Engine → Client 순서로** 빌드합니다. (Engine이 `Libraries/Lib/Engine/Engine.lib`을 만들고 Client가 링크합니다.)

> Debug와 Release가 같은 `Engine.lib` · 출력 경로를 사용하므로, 구성을 바꾼 뒤에는 Engine부터 다시 빌드해야 합니다.

### 디버그 키

| 키 | 기능 |
|---|---|
| F7 | 경로 스무딩 방식 순환 (Corridor · Funnel · LOS 및 비교용 변형) |
| F8 | 경로 진단 오버레이 |
| F9 | 경로 스무딩 벤치마크 실행 (결과 CSV 저장) |
| F11 | 충돌 처리 방식 전환 (기존 화면 QuadTree / Loose QuadTree) |

---

## 프로젝트 구조

```
DirectX11-Engine-Client/
├── AssimpTool/          # FBX (모델 · 애니메이션) 변환 도구
├── Client/              # 게임 코드 (캐릭터, 몬스터, 아이템, UI, 씬)
├── Engine/              # 엔진 코드 (정적 라이브러리)
│   ├── 00. Engine/        # 기본 구조, 인터페이스, 리소스
│   ├── 01. Graphics/      # Shader, Buffer 등 DirectX 래핑
│   ├── 02. Managers/      # Game, Graphics, Scene, Input, Event 매니저
│   ├── 03. GameObject/    # 게임 오브젝트
│   ├── 04. Component/     # 컴포넌트 (Animation, NavMesh, Collision, BehaviorTree, 상태 머신)
│   ├── 05. ComputeShader/ # Raw · Structured · Texture Buffer
│   ├── 06. UI & Effect/   # Direct2D UI, Billboard 등 이펙트
│   ├── 98. Utils/         # 수학, ImGui 등 유틸
│   └── 99. Headers/       # pch, define, 공통 타입
├── Libraries/           # Assimp, DirectXTex, FX11, FMOD, Engine lib
├── Resources/           # 텍스처, 모델, 사운드
├── Shaders/             # HLSL
└── GameCoding.sln
```
