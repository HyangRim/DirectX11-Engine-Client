#pragma once
#include "pch.h"

struct NavTriangle
{
    Vec3 vertices[3];        // 삼각형의 세 꼭짓점
    Vec3 normal;            // 면의 법선 벡터
    Vec3 center;            // 삼각형 중심점
    int id;                 // 고유 식별자
    vector<int> neighbors;  // 인접한 삼각형들의 ID
    bool walkable = true;   // 걸을 수 있는지 여부
};

struct NavNode
{
    Vec3 position;          // 노드 위치
    float gCost = 0.0f;     // 시작점부터의 실제 거리
    float hCost = 0.0f;     // 목표점까지의 추정 거리
    float fCost = 0.0f;     // gCost + hCost (총 비용)
    int triangleId = -1;    // 소속 삼각형 ID
    int parentNodeId = -1;  // 부모 노드 ID (경로 역추적용)
};

struct NavPath
{
    vector<Vec3> waypoints;  // 경로상의 웨이포인트들
    bool isValid = false;    // 유효한 경로인지 여부
    float totalDistance = 0.0f; // 총 경로 길이
};
