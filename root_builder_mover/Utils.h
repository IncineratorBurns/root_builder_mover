#pragma once
#include <wx/vector.h>

namespace Utils
{
	wxVector<wxString> Tokenize(const wxString& a_src, const wxString& a_delim);
	std::pair<wxString, wxString> ConfigEntryTokenize(const wxString& a_src);
}
