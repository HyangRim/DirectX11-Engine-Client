#pragma once

template<typename ..._Args>
using Function = std::function<void(_Args...)>;


template<typename ..._Args>
inline void Delegate::Delegate<_Args...>::Push(Function&& _func)
{
	m_functionList.push_back(_func);
}

template<typename ..._Args>
inline void Delegate::Delegate<_Args...>::Pop(Function&& _func)
{
	for (int index = 0; index < m_functionList.size(); ++index)
	{
		if (m_functionList[index].target_type() == _func.target_type())
		{
			m_functionList.erase(std::next(m_functionList.begin(), index));
			break;
		}
	}
}

template<typename ..._Args>
inline void Delegate::Delegate< _Args... >::Reset()
{
	m_functionList.clear();
}

template<typename ..._Args>
inline void Delegate::Delegate<_Args...>::operator+=(const Function& _func)
{
	m_functionList.push_back(_func);
}

template<typename ..._Args>
inline void Delegate::Delegate<_Args...>::operator+=(const Function&& _func)
{
	m_functionList.push_back(_func);
}

template<typename ..._Args>
inline void Delegate::Delegate<_Args...>::operator-=(const Function& _func)
{
	for (int index = 0; index < m_functionList.size(); index++)
	{
		if (m_functionList[index].target_type() == _func.target_type())
		{
			m_functionList.erase(std::next(m_functionList.begin(), index));
			break;
		}
	}
}

template<typename ..._Args>
inline void Delegate::Delegate<_Args...>::operator-=(const Function&& _func)
{
	for (int index = 0; index < m_functionList.size(); index++)
	{
		if (m_functionList[index].target_type() == _func.target_type())
		{
			m_functionList.erase(std::next(m_functionList.begin(), index));
			break;
		}
	}
}

template<typename ..._Args>
inline void Delegate::Delegate<_Args...>::operator=(const Function& _func)
{
	m_functionList.clear();
	m_functionList.push_back(_func);
}

template<typename ..._Args>
inline void Delegate::Delegate<_Args...>::operator()(_Args... _types)
{
	for (auto& func : m_functionList)
	{
		func(std::forward<_Args>(_types)...);
	}
}