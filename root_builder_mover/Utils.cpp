#include "Utils.h"

#include <wx/tokenzr.h>

wxVector<wxString> Utils::Tokenize(const wxString& a_src, const wxString& a_delim)
{
	wxVector<wxString> l_ret;
	wxStringTokenizer l_tokenizer(a_src, a_delim);
	while (l_tokenizer.HasMoreTokens())
		l_ret.push_back(l_tokenizer.GetNextToken());
	
	return l_ret;
}

wxVector<wxString> Utils::ReplaceDelimiterAndTokenize(wxString a_src, const wxString& a_olddelim,
	const wxString& a_newdelim)
{
	a_src.Replace(a_olddelim, a_newdelim, true);
	return Utils::Tokenize(a_src, "\n");
}

std::pair<wxString, wxString> Utils::ConfigEntryTokenize(const wxString& a_src)
{
	auto l_delim_pos = a_src.find("=");

	return std::make_pair(a_src.SubString(0, l_delim_pos), a_src.SubString(l_delim_pos + 1, a_src.size()));
}
