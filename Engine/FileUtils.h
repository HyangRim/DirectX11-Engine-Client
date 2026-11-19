#pragma once

enum FileMode : uint8
{
	Write,
	Read,
};

class FileUtils
{
public:
	FileUtils();
	~FileUtils();

	void Open(wstring filePath, FileMode mode);

	template<typename T>
	void Write(const T& data)
	{
		DWORD numOfBytes = 0;
		//표준에서 제공하는 걸로 파일을 써주기. 
		//Return받는게 numOfBytes

		//String은 sizeof로 하는 게 맞나? 따라서 버전을 2개 쓴다. 
		// WriteFile 을 assert 안에 두면 Release (NDEBUG) 에서 호출 자체가 사라진다
		const BOOL ok = ::WriteFile(_handle, &data, sizeof(T), (LPDWORD)&numOfBytes, nullptr);
		assert(ok);
		(void)ok;
	}

	//String용도. 템플릿 특수화. (일종의 예외처리)
	template<>
	void Write<string>(const string& data)
	{
		return Write(data);
	}

	void Write(void* data, uint32 dataSize);
	void Write(const string& data);

	template<typename T>
	void Read(OUT T& data)
	{
		DWORD numOfBytes = 0;
		// ReadFile 을 assert 안에 두면 Release (NDEBUG) 에서 호출 자체가 사라진다
		const BOOL ok = ::ReadFile(_handle, &data, sizeof(T), (LPDWORD)&numOfBytes, nullptr);
		assert(ok);
		(void)ok;
	}

	template<typename T>
	T Read()
	{
		T data;
		Read(data);
		return data;
	}

	void Read(void** data, uint32 dataSize);
	void Read(OUT string& data);

private:
	HANDLE _handle = INVALID_HANDLE_VALUE;
};

