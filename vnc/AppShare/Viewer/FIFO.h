#if !defined(AFX_FIFO_H__50B8544A_4FE0_478E_935D_CDB2F1306656__INCLUDED_)
#define AFX_FIFO_H__50B8544A_4FE0_478E_935D_CDB2F1306656__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FIFO.h : header file
//

#define DEFAULT_FIFO_SIZE 1024*2

/////////////////////////////////////////////////////////////////////////////
// CFIFO 
// version 2 - this version use cycle buffer instead of v1's memcpy in buffer
// Author: Kevin, (C) Aculearn, 2004.12.1

class CFIFO
{
// Construction
public:
	CFIFO();
	~CFIFO();

// Attribute
private:
	char*	m_pBuffer;		// pointer to fifo buffer
	int		m_Size;			// fifo size
	int		m_ReadIndex;	// position of first byte be used
	int		m_WriteIndex;	// position of next available byte in buffer
	int		m_Length;		// fifo length (bytes in fifo)

	CRITICAL_SECTION	m_csLock;

// Operations
public:
	BOOL Create(int size = DEFAULT_FIFO_SIZE);
	void Clear();

	int	Put(const char* buffer, int len);	// Caller should allocate memory
	int	Get(char* buffer, int len);			// Caller should allocate memory
	int Peek(char* buffer, int len);		// Caller should allocate memory

	// following fuction is not save to call in multithread env.
	// you can not count on the returned result, for other thread may already
	// updated the fifo content before you do the next step.
	// they are here is for single thread call or debugging purpose.
	BOOL IsEmpty();
	int GetSize();
	int GetLength();

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIFO_H__50B8544A_4FE0_478E_935D_CDB2F1306656__INCLUDED_)
