#include "Utils.h"

#include <array>
#include <wx/stream.h>
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

std::vector<char> Utils::Read(wxInputStream& a_src)
{
	std::vector<char> l_data;
	std::array<char, 4096> l_buff;
	while (a_src.IsOk() && !a_src.Eof())
	{
		size_t l_bytes_read = 4096;
		auto l_ret = a_src.ReadAll((void*)&l_buff[0], 4096);
		if (!l_ret)
			l_bytes_read = a_src.LastRead();

		auto l_cursor_into_ldata = l_data.size();
		l_data.resize(l_data.size() + l_bytes_read);
		for (auto i = 0; i < l_bytes_read; ++i)
			l_data[l_cursor_into_ldata + i] = l_buff[i];
	}
	return l_data;
}
