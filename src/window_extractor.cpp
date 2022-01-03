#include "window_extractor.h"

namespace d2::ui {
    ExtractorWindows::ExtractorWindows() {
        SetSizeHints(wxDefaultSize, wxDefaultSize);

        wxBoxSizer* sizer;
        sizer = new wxBoxSizer(wxVERTICAL);

        progress = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
        progress->SetValue(0);
        sizer->Add(progress, 0, wxALL | wxEXPAND, 5);

        wxGridSizer* buttonSizer;
        buttonSizer = new wxGridSizer(0, 2, 0, 0);

        mode_single = new wxButton(this, wxID_ANY, wxT("Load Single Package"), wxDefaultPosition, wxDefaultSize, 0);
        buttonSizer->Add(mode_single, 0, wxALL | wxALIGN_LEFT, 5);

        mode_folder = new wxButton(this, wxID_ANY, wxT("Load Package Folder"), wxDefaultPosition, wxDefaultSize, 0);
        buttonSizer->Add(mode_folder, 0, wxALL | wxALIGN_RIGHT, 5);

        sizer->Add(buttonSizer, 1, wxEXPAND, 5);

        output = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr, 0);
        sizer->Add(output, 0, wxALL | wxEXPAND, 5);

        spacer = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
        sizer->Add(spacer, 0, wxEXPAND | wxALL, 5);

        m_staticText1 = new wxStaticText(this, wxID_ANY, wxT("Destiny Tool v1.0"), wxDefaultPosition, wxDefaultSize, 0);
        m_staticText1->Wrap(-1);
        sizer->Add(m_staticText1, 0, wxALIGN_RIGHT | wxRIGHT, 5);

        SetSizer(sizer);
        this->Layout();
        sizer->Fit(this);

        Centre(wxBOTH);

        mode_single->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ExtractorWindows::load_package_single), NULL, this);
        mode_folder->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ExtractorWindows::load_package_folder), NULL, this);
    }
}
