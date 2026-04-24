#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "DebugMenu.h"
#include "GameObject.hpp"

class Component
{
	friend GameObject;
public:
	class DataAccessor
	{
	public:
		DataAccessor(char* ptr) : m_isRead(ptr), m_size(64), m_cur(0) {
			m_ptr = m_isRead ? ptr : new char[m_size];
		}
		~DataAccessor() { if (!m_isRead) { delete[] m_ptr; } }
		// データの読み書きを実行
		template<class T> void Access(T* ptr) {
			if (m_isRead)
				Read<T>(ptr);
			else
				Write<T>(ptr);
		}
		int GetWriteSize() { return m_cur; }
		char* GetData() { return m_ptr; }

	private:
		// 読み込み
		template<class T> void Read(T* ptr) {
			int size = sizeof(T);
			memcpy(ptr, m_ptr, size);
			m_ptr += size;
		}
		// 書き込み
		template<class T> void Write(T* ptr) {
			int size = sizeof(T);
			// 確保済みのサイズより大きくなりそうであれば再確保
			if (m_size < m_cur + size) {
				char* work = m_ptr;
				m_ptr = new char[m_size <<= 1];
				memcpy(m_ptr, work, m_cur);
				delete[] work;
			}
			memcpy(m_ptr + m_cur, ptr, size);
			m_cur += size;
		}
	private:
		bool m_isRead;
		char* m_ptr;
		int m_size;
		int m_cur;
	};
public:
	Component();
	virtual ~Component();
	virtual void Execute();

	virtual void ReadWrite(DataAccessor* data);
#if _DEBUG
	virtual void Debug(debug::Window* window);
#endif
protected:
	GameObject* transform;
};


#endif // __COMPONENT_H__