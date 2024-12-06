#include "MultiLineRenderer.h"

#include <wx/dc.h>
#include <wx/log.h>

MyCustomRenderer::MyCustomRenderer(wxDataViewCellMode mode): wxDataViewCustomRenderer("string", mode, wxALIGN_CENTER)
{ }

bool MyCustomRenderer::Render(wxRect rect, wxDC* dc, int state)
{
	dc->SetBrush(*wxLIGHT_GREY_BRUSH);
	dc->SetPen(*wxTRANSPARENT_PEN);

	rect.Deflate(2);
	dc->DrawRoundedRectangle(rect, 5);

	RenderText(m_value,
	           0, // no offset
	           wxRect(dc->GetTextExtent(m_value)).CentreIn(rect),
	           dc,
	           state);
	return true;
}

bool MyCustomRenderer::ActivateCell(const wxRect&, wxDataViewModel*, const wxDataViewItem&, unsigned int,
	const wxMouseEvent* mouseEvent)
{
	wxString position;
	if (mouseEvent)
		position = wxString::Format("via mouse at %d, %d", mouseEvent->m_x, mouseEvent->m_y);
	else
		position = "from keyboard";
	wxLogMessage("MyCustomRenderer ActivateCell() %s", position);
	return false;
}

wxSize MyCustomRenderer::GetSize() const
{
	return GetView()->FromDIP(wxSize(60, 20));
}

bool MyCustomRenderer::SetValue(const wxVariant& value)
{
	m_value = value.GetString();
	return true;
}

bool MyCustomRenderer::GetValue(wxVariant&) const
{ return true; }

wxString MyCustomRenderer::GetAccessibleDescription() const
{
	return m_value;
}

bool MyCustomRenderer::HasEditorCtrl() const
{ return true; }

wxWindow* MyCustomRenderer::CreateEditorCtrl(wxWindow* parent, wxRect labelRect, const wxVariant& value)
{
	wxTextCtrl* text = new wxTextCtrl(parent, wxID_ANY, value,
	                                  labelRect.GetPosition(),
	                                  labelRect.GetSize(),
	                                  wxTE_PROCESS_ENTER);
	text->SetInsertionPointEnd();

	return text;
}

bool MyCustomRenderer::GetValueFromEditorCtrl(wxWindow* ctrl, wxVariant& value)
{
	wxTextCtrl* text = wxDynamicCast(ctrl, wxTextCtrl);
	if (!text)
		return false;

	value = text->GetValue();

	return true;
}

MultiLineCustomRenderer::MultiLineCustomRenderer(): wxDataViewCustomRenderer("string", wxDATAVIEW_CELL_INERT, 0)
{ }

bool MultiLineCustomRenderer::Render(wxRect rect, wxDC* dc, int state)
{
	RenderText(m_value, 0, rect, dc, state);
	return true;
}

wxSize MultiLineCustomRenderer::GetSize() const
{
	wxSize txtSize = GetTextExtent(m_value);
	int lines = m_value.Freq('\n') + 1;
	txtSize.SetHeight(txtSize.GetHeight() * lines + 10);
	return txtSize;
}

bool MultiLineCustomRenderer::SetValue(const wxVariant& value)
{
	m_value = value.GetString();
	//m_value.Replace(" ", "\n");
	return true;
}

bool MultiLineCustomRenderer::GetValue(wxVariant&) const
{ return true; }

wxString MultiLineCustomRenderer::GetAccessibleDescription() const
{
	return m_value;
}
