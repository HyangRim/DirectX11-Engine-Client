#pragma once

class Utils
{
public:
	static bool StartsWith(string _str, string _comp);
	static bool StartsWith(wstring _str, wstring _comp);

	static void Replace(OUT string& _str, string _comp, string _rep);
	static void Replace(OUT wstring& _str, wstring _comp, wstring _rep);


	static wstring ToWString(string _value);
	static string ToString(wstring _value);
};

