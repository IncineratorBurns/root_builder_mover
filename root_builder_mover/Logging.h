#pragma once
#include <array>
#include <system_error>
#include <wx/string.h>
#include <wx/vector.h>
#include <wx/artprov.h>

class wxVariant;
class wxDataViewListCtrl;

bool PrintLog(const std::error_code& l_ec, const wxString& a_meta, const char* a_func_name, const wxString& a_msg1, const wxString& a_msg2);
bool PrintLog(const std::error_code& l_ec, const wxString& a_meta, const char* a_func_name, const wxString& a_msg);
bool PrintInfo(const wxString& a_message, const wxString& a_meta);
bool PrintError(const wxString& a_message, const wxString& a_meta);

const char* const META_CORE = "[ CORE ]";

enum LogItemType
{
	Info, Success, Error, All
};

constexpr std::array<const char*, 4> g_LogItemTypeStrings{ "Info", "Success", "Error", "All" };

enum LogItemValueIndex
{
	SerialNumber, Type, Meta, Into
};

struct LogItem
{
    long m_sn;
    LogItemType m_type;
    wxString m_meta;
    wxString m_info;

	wxString TypeLiteral() const;
	wxVector<wxVariant> ToVector() const;
};

class Logger
{
public:
	static Logger& Instance(wxDataViewListCtrl* a_ctrl = nullptr);

	wxDataViewListCtrl* m_pLogCtrl = nullptr;
	std::vector<LogItem> m_all_events;
	LogItemType m_filter = LogItemType::All;

	long GetNewSN();

	void LogAndNotifyCtrl(const LogItem& a_logItem);
	const wxBitmap& GetBtimap(const LogItem& a_item) const;
	auto GetCtrl() -> decltype(m_pLogCtrl) { return m_pLogCtrl; }
	void Clear();
	void Filter(LogItemType a_event_type);
	void Repopulate();

private:
	Logger() {}
	long m_counter;

	wxBitmap m_bitmap_info;
	wxBitmap m_bitmap_success;
	wxBitmap m_bitmap_error;


public:
	Logger(const Logger&) = delete;
	void operator=(const Logger&) = delete;

};