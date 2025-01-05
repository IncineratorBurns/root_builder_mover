#include "Utils.h"

#include <array>
#include <wx/stream.h>
#include <wx/tokenzr.h>
#include <wx/url.h>

#include "Logging.h"

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

std::unique_ptr<wxInputStream> Utils::GetStreamForURL(const wxString& a_url)
{
	wxURL l_url(a_url);

	auto l_error = l_url.GetError();

	if (l_error != wxURL_NOERR)
	{
		auto l_err_msg = std::array<std::pair<const char*, const char*>, 7>{
			std::make_pair("wxURL_NOERR", "No error."),
				std::make_pair("wxURL_SNTXERR", "Syntax error in the URL string."),
				std::make_pair("wxURL_NOPROTO", "Found no protocol which can get this URL."),
				std::make_pair("wxURL_NOHOST", "A host name is required for this protocol."),
				std::make_pair("wxURL_NOPATH", "A path is required for this protocol."),
				std::make_pair("wxURL_CONNERR", "Connection error."),
				std::make_pair("wxURL_PROTOERR", "An error occurred during negotiation.")
		};

		PrintError(wxString::Format("Failed to get the update file. %s (%s)", l_err_msg[l_error].second,
			l_err_msg[l_error].first),
			META_UPDATER);
		return nullptr;
	}

	wxInputStream* l_inputStream = l_url.GetInputStream();

	if (!l_inputStream || !l_inputStream->IsOk())
	{
		PrintError("Can't proceed with bundled.cfg update as the updated file couldn't be downloaded.",
			META_UPDATER);

		delete l_inputStream;
		return nullptr;
	}

	return std::unique_ptr<wxInputStream>(l_inputStream);
}
