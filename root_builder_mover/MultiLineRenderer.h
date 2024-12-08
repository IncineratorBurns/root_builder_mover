#pragma once

#include <wx/dataview.h>

// ----------------------------------------------------------------------------
// MultiLineCustomRenderer
// ----------------------------------------------------------------------------

class MultiLineCustomRenderer : public wxDataViewCustomRenderer
{
public:
    // a simple renderer that wraps each word on a new line
    explicit MultiLineCustomRenderer();

    bool Render(wxRect rect, wxDC* dc, int state) override;

    wxSize GetSize() const override;

    bool SetValue(const wxVariant& value) override;

    bool GetValue(wxVariant& WXUNUSED(value)) const override;

#if wxUSE_ACCESSIBILITY
    wxString GetAccessibleDescription() const override;
#endif // wxUSE_ACCESSIBILITY

private:
    wxString m_value;
};