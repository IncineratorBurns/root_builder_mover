#include "Logging.h"

#include <system_error>
#include <wx/bitmap.h>
#include <wx/artprov.h>
#include <wx/dataview.h>
#include <wx/log.h>

bool PrintLog(const std::error_code& l_ec, const wxString& a_meta, const char* a_func_name, const wxString& a_msg1,
              const wxString& a_msg2)
{
	LogItem l_logItem
	{
		Logger::Instance().GetNewSN(),
		l_ec.value() == 0 ? LogItemType::Success : LogItemType::Error,
		a_meta,
		l_ec.value() == 0 ? 
		wxString::Format("ACT: %s\nSRC: %s\nDST: %s", a_func_name, a_msg1, a_msg2) :
		wxString::Format("ACT: %s\nSRC: %s\nDST: %s\nERR: %s", a_func_name, a_msg1, a_msg2, l_ec.message())
	};

	Logger::Instance().LogAndNotifyCtrl(l_logItem);

	return l_ec.value() == 0;
}

bool PrintLog(const std::error_code& l_ec, const wxString& a_meta, const char* a_func_name, const wxString& a_msg)
{
	LogItem l_logItem
	{
		Logger::Instance().GetNewSN(),
		l_ec.value() == 0 ? LogItemType::Success : LogItemType::Error,
		a_meta,
		l_ec.value() == 0 ?
		wxString::Format("ACT: %s\nTGT: %s", a_func_name, a_msg) :
		wxString::Format("ACT: %s\nTGT: %s\nERR: %s", a_func_name, a_msg, l_ec.message())
	};

	Logger::Instance().LogAndNotifyCtrl(l_logItem);

	return l_ec.value() == 0;
}

bool PrintInfo(const wxString& a_message, const wxString& a_meta)
{
	LogItem l_logItem
	{
		Logger::Instance().GetNewSN(),
		LogItemType::Info,
		a_meta,
		wxString::Format("INF: %s", a_message)
	};

	Logger::Instance().LogAndNotifyCtrl(l_logItem);

	return true;
}

bool PrintError(const wxString& a_message, const wxString& a_meta)
{
	LogItem l_logItem
	{
		Logger::Instance().GetNewSN(),
		LogItemType::Error,
		a_meta,
		wxString::Format("ERR: %s", a_message)
	};

	Logger::Instance().LogAndNotifyCtrl(l_logItem);

	return true;
}

wxString LogItem::TypeLiteral() const
{
	return g_LogItemTypeStrings[m_type];
}

wxVector<wxVariant> LogItem::ToVector() const
{
	wxVector<wxVariant> l_ret;
	l_ret.push_back(wxVariant(wxString::Format("%d", m_sn)));
	l_ret.push_back( wxVariant(wxDataViewIconText("", Logger::Instance().GetBtimap(*this))));
	l_ret.push_back(m_meta);
	l_ret.push_back(m_info);
	return l_ret;
}

Logger& Logger::Instance(wxDataViewListCtrl* a_ctrl)
{
	static Logger l_instance;

	if (l_instance.m_pLogCtrl == nullptr && a_ctrl != nullptr)
	{
		l_instance.m_pLogCtrl = a_ctrl;

		l_instance.m_bitmap_info = wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_FRAME_ICON);
		l_instance.m_bitmap_success = wxArtProvider::GetBitmap(wxART_TICK_MARK, wxART_FRAME_ICON);
		l_instance.m_bitmap_error = wxArtProvider::GetBitmap(wxART_ERROR, wxART_FRAME_ICON);
	}

	return l_instance;
}

long Logger::GetNewSN()
{
	return m_counter++;
}

void Logger::LogAndNotifyCtrl(const LogItem& a_logItem)
{
	m_all_events.push_back(a_logItem);

	if (m_filter == LogItemType::All || a_logItem.m_type == m_filter)
	{
		auto l_vals = a_logItem.ToVector();
		m_pLogCtrl->AppendItem(l_vals);
	}
}

const wxBitmap& Logger::GetBtimap(const LogItem& a_item) const
{
	switch (a_item.m_type)
	{
	case Info:
		return m_bitmap_info;
	case Success:
		return m_bitmap_success;
	case Error:
		return m_bitmap_error;
	}
}

void Logger::Clear()
{
	m_pLogCtrl->DeleteAllItems();
	m_all_events.clear();
	m_counter = 0;
}

void Logger::Filter(LogItemType a_event_type)
{
	m_filter = a_event_type;
	Repopulate();
}

void Logger::Repopulate()
{
	m_pLogCtrl->DeleteAllItems();
	for (auto& i : m_all_events)
		if (m_filter == LogItemType::All || i.m_type == m_filter)
			m_pLogCtrl->AppendItem(i.ToVector());
}


