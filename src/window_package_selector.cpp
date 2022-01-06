#include "window.h"

namespace d2::ui {

    PackageSelector::PackageSelector(wxWindow* parent, const wxFileName& path) : wxDialog(parent, wxID_ANY, "Select Packages..."){
        SetSizeHints(wxDefaultSize, wxDefaultSize);

        wxBoxSizer* sizerMain;
        sizerMain = new wxBoxSizer(wxVERTICAL);

        list = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize( 250,300 ), 0, nullptr, wxLB_HSCROLL | wxLB_EXTENDED | wxLB_NEEDED_SB | wxLB_SORT);
        sizerMain->Add(list, 0, wxALL | wxEXPAND, 5);

        wxGridSizer* sizer;
        sizer = new wxGridSizer(0, 2, 0, 0);

        select = new wxButton(this, wxID_ANY, wxT("Select"), wxDefaultPosition, wxDefaultSize, 0);
        select->Disable();
        sizer->Add(select, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);

        cancel = new wxButton(this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
        sizer->Add(cancel, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);

        sizerMain->Add(sizer, 1, wxEXPAND, 5);

        SetSizer(sizerMain);
        this->Layout();
        sizerMain->Fit(this);

        Centre(wxBOTH);

        select->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PackageSelector::on_select), nullptr, this );
        cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PackageSelector::on_cancel), nullptr, this );

        list->Connect(wxEVT_LISTBOX, wxCommandEventHandler(PackageSelector::on_list), nullptr, this);
    }

    void PackageSelector::add_name(const wxString &name) {
        list->Append(name);
    }
}