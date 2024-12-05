#pragma once
#include <wx/vector.h>

namespace Utils
{
	wxVector<wxString> Tokenize(const wxString& a_src, const wxString& a_delim);
	// a_src by value is indended - need a copy here.
	wxVector<wxString> ReplaceDelimiterAndTokenize(wxString a_src, const wxString& a_olddelim = "||", const wxString& a_newdelim = "\n");
	std::pair<wxString, wxString> ConfigEntryTokenize(const wxString& a_src);
}
