#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/statline.h>
#include <wx/filepicker.h>

#include "types.h"

namespace d2::ui {
    class ExtractorWindows : public wxDialog {

    protected:
        wxGauge* progress;
        wxDirPickerCtrl* dir;
        wxButton* mode_single;
        wxButton* mode_folder;
        wxTextCtrl* output;
        wxStaticLine* spacer;
        wxStaticText* m_staticText1;

        void load_package_single(wxCommandEvent& event);
        void load_package_folder(wxCommandEvent& event);
        void on_dir_changed(wxFileDirPickerEvent& event);
    public:
        ExtractorWindows();

        std::string get_folder();
        void add_output_line(std::string line);
        void clear_output();
    };

    class PackageSelector : public wxDialog {

    protected:
        wxListBox* list;
        wxButton* select;
        wxButton* cancel;

        void on_select(wxCommandEvent& event);
        void on_cancel(wxCommandEvent& event);
        void on_list(wxCommandEvent &event);

    public:
        PackageSelector(wxWindow* parent, const wxFileName& path);

        void add_name(const wxString &name);
    };
}


