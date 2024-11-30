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

std::pair<wxString, wxString> Utils::ConfigEntryTokenize(const wxString& a_src)
{
	auto l_delim_pos = a_src.find("=");

	return std::make_pair(a_src.SubString(0, l_delim_pos), a_src.SubString(l_delim_pos + 1, a_src.size()));
}
