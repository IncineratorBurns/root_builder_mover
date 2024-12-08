#include "MultiLineRenderer.h"

#include <wx/dc.h>

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
