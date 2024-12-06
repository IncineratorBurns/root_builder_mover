#pragma once

#include <wx/dataview.h>

#include "Logging.h"

// ----------------------------------------------------------------------------
// MyCustomRenderer
// ----------------------------------------------------------------------------

class wxDataViewModel;
class wxDataViewItem;
class wxMouseEvent;

class MyCustomRenderer : public wxDataViewCustomRenderer
{
public:
    // This renderer can be either activatable or editable, for demonstration
    // purposes. In real programs, you should select whether the user should be
    // able to activate or edit the cell and it doesn't make sense to switch
    // between the two -- but this is just an example, so it doesn't stop us.
    explicit MyCustomRenderer(wxDataViewCellMode mode);

    virtual bool Render(wxRect rect, wxDC* dc, int state) override;

    virtual bool ActivateCell(const wxRect& WXUNUSED(cell),
                              wxDataViewModel* WXUNUSED(model),
                              const wxDataViewItem& WXUNUSED(item),
                              unsigned int WXUNUSED(col),
                              const wxMouseEvent* mouseEvent) override;

    virtual wxSize GetSize() const override;

    virtual bool SetValue(const wxVariant& value) override;

    virtual bool GetValue(wxVariant& WXUNUSED(value)) const override;

#if wxUSE_ACCESSIBILITY
    virtual wxString GetAccessibleDescription() const override;
#endif // wxUSE_ACCESSIBILITY

    virtual bool HasEditorCtrl() const override;

    virtual wxWindow*
        CreateEditorCtrl(wxWindow* parent,
            wxRect labelRect,
            const wxVariant& value) override;

    virtual bool
        GetValueFromEditorCtrl(wxWindow* ctrl, wxVariant& value) override;

private:
    wxString m_value;
};


// ----------------------------------------------------------------------------
// MultiLineCustomRenderer
// ----------------------------------------------------------------------------

class MultiLineCustomRenderer : public wxDataViewCustomRenderer
{
public:
    // a simple renderer that wraps each word on a new line
    explicit MultiLineCustomRenderer();

    virtual bool Render(wxRect rect, wxDC* dc, int state) override;

    virtual wxSize GetSize() const override;

    virtual bool SetValue(const wxVariant& value) override;

    virtual bool GetValue(wxVariant& WXUNUSED(value)) const override;

#if wxUSE_ACCESSIBILITY
    virtual wxString GetAccessibleDescription() const override;
#endif // wxUSE_ACCESSIBILITY

private:
    wxString m_value;
};