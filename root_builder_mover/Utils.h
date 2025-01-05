#pragma once
#include <memory>
#include <wx/vector.h>

class wxInputStream;

namespace Utils
{
	wxVector<wxString> Tokenize(const wxString& a_src, const wxString& a_delim);
	// a_src by value is indended - need a copy here.
	wxVector<wxString> ReplaceDelimiterAndTokenize(wxString a_src, const wxString& a_olddelim = "||", const wxString& a_newdelim = "\n");
	std::pair<wxString, wxString> ConfigEntryTokenize(const wxString& a_src);
	std::vector<char> Read(wxInputStream& a_src);
	std::unique_ptr<wxInputStream> GetStreamForURL(const wxString& a_url);
}
