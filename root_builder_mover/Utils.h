#pragma once
#include <wx/vector.h>

namespace Utils
{
	wxVector<wxString> Tokenize(const wxString& a_src, const wxString& a_delim);
	// a_src by value is indended - need a copy here.
	wxVector<wxString> ReplaceDelimiterAndTokenize(wxString a_src, const wxString& a_olddelim = "||", const wxString& a_newdelim = "\n");
	std::pair<wxString, wxString> ConfigEntryTokenize(const wxString& a_src);

	template<typename T>
	class Singleton
	{
	public:
		static Singleton& Instance(T* a_ctrl = nullptr)
		{
			static Singleton l_instance;

			if (l_instance.m_pLogCtrl == nullptr && a_ctrl != nullptr)
				l_instance.m_pLogCtrl = a_ctrl;

			return l_instance;
		}

		T* m_pLogCtrl = nullptr;

	private:
		Singleton() {}

	public:
		Singleton(const Singleton&) = delete;
		void operator=(const Singleton&) = delete;
	};
}
