#include "window.h"
#include "file_helper.h"

namespace d2::ui {

    ExtractorWindows::ExtractorWindows() : wxDialog(nullptr, wxID_ANY, wxT("Destiny Tool"), wxDefaultPosition, wxSize(280, -1), wxDEFAULT_DIALOG_STYLE) {
        SetSizeHints(wxDefaultSize, wxDefaultSize);

        wxBoxSizer* sizer;
        sizer = new wxBoxSizer(wxVERTICAL);

        progress = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
        progress->SetValue(0);
        sizer->Add(progress, 0, wxALL | wxEXPAND, 5);

        dir = new wxDirPickerCtrl(this, wxID_ANY, wxEmptyString, wxT("Select a package folder..."), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE | wxDIRP_DIR_MUST_EXIST);
        sizer->Add(dir, 0, wxALL | wxEXPAND, 5);

        wxGridSizer* buttonSizer;
        buttonSizer = new wxGridSizer(0, 2, 0, 0);

        mode_single = new wxButton(this, wxID_ANY, wxT("Individual Packages"), wxDefaultPosition, wxSize(125, -1), 0);
        mode_single->SetDefault();
        buttonSizer->Add(mode_single, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        mode_folder = new wxButton(this, wxID_ANY, wxT("Entire Package Folder"), wxDefaultPosition, wxSize(125, -1), 0);
        buttonSizer->Add(mode_folder, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        sizer->Add(buttonSizer, 1, wxEXPAND, 5);

        output = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition,wxSize(-1, 100), wxTE_MULTILINE|wxTE_READONLY );
        sizer->Add( output, 0, wxALL | wxEXPAND, 5 );

        spacer = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
        sizer->Add(spacer, 0, wxEXPAND | wxALL, 5);

        m_staticText1 = new wxStaticText(this, wxID_ANY, wxT("v1.0"), wxDefaultPosition, wxDefaultSize, 0);
        m_staticText1->Wrap(-1);
        sizer->Add(m_staticText1, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 5);

        SetSizer(sizer);
        this->Layout();
        sizer->Fit(this);

        Centre(wxBOTH);

        mode_single->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ExtractorWindows::load_package_single), nullptr, this);
        mode_folder->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ExtractorWindows::load_package_folder), nullptr, this);
        dir->Connect(wxEVT_DIRPICKER_CHANGED, wxFileDirPickerEventHandler(ExtractorWindows::on_dir_changed), nullptr, this);
    }

    void ExtractorWindows::add_output_line(std::string line) {
        output->AppendText(wxString(line + '\n'));
    }

    void ExtractorWindows::clear_output() {
        output->Clear();
    }

    std::string ExtractorWindows::get_folder() {
        return dir->GetPath().utf8_string();
    }
}
