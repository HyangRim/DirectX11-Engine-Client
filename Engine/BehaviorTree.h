#pragma once

#include "Delegate.h" 

// 노드의 상태
enum class NodeState 
{
    SUCCESS,
    FAILURE,
    RUNNING
};

// 1. 모든 노드의 기본 인터페이스
class INode 
{
public:
    virtual ~INode() = default;
    virtual NodeState Evaluate() = 0; // 실행 및 결과 반환
};

// 2. Action Node (수정됨)
class ActionNode : public INode
{
    // NodeState를 반환하는 함수 타입 정의
    using ActionFunc = std::function<NodeState()>;

    ActionFunc m_action;

public:
    // 생성자에서 함수 객체를 받음
    ActionNode(ActionFunc action) : m_action(std::move(action)) {}

    virtual NodeState Evaluate() override
    {
        if (m_action)
            return m_action(); // 바인딩된 함수 실행

        return NodeState::FAILURE;
    }
};

// 3. Selector Node
class SelectorNode : public INode 
{
    vector<shared_ptr<INode>> m_children;

public:
    void AddChild(shared_ptr<INode> node) { m_children.push_back(node); }

    virtual NodeState Evaluate() override 
    {
        for (auto& child : m_children) 
        {
            NodeState result = child->Evaluate();
            if (result != NodeState::FAILURE) return result; // Success or Running
        }
        return NodeState::FAILURE;
    }
};

// 4. Sequence Node 
class SequenceNode : public INode 
{
    vector<shared_ptr<INode>> m_children;

public:
    void AddChild(shared_ptr<INode> node) { m_children.push_back(node); }

    virtual NodeState Evaluate() override 
    {
        for (auto& child : m_children) 
        {
            NodeState result = child->Evaluate();
            if (result != NodeState::SUCCESS) return result; // Failure or Running
        }
        return NodeState::SUCCESS;
    }
};

class BehaviorTree
{
public:
    BehaviorTree() = default;
    ~BehaviorTree() = default;

    // 루트 노드 설정 (보통 SelectorNode가 루트가 됨)
    void SetRootNode(shared_ptr<INode> root) { m_root = root; }

    // 매 프레임 호출: 트리를 실행
    void Update()
    {
        if (m_root)
        {
            m_root->Evaluate();
        }
    }

private:
    shared_ptr<INode> m_root;

};

