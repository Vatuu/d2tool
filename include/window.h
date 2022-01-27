#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/statline.h>
#include <wx/filepicker.h>

#include "types.h"
#include "callbacks.h"

namespace d2::ui {

    struct ExtractorWindow;

    struct Callbacks : extract::ExtractCallbacks {
        explicit Callbacks(ExtractorWindow *window);
        ~Callbacks() = default;

    private:
        ExtractorWindow *window;
        u32 progressAmount;

        void add_line(const std::string& line);
        void progress_bar();
        void clear();

    public:
        void on_error(d2::extract::Error errorType, std::variant<Package*, Entry*, Block*, std::monostate> object, std::optional<std::string> additionalInfo) override;
        void log(const std::string& line) override;
        void on_extract_start() override;
        void on_extract_end(bool errors) override;
        bool on_package_start(const Package *package, const std::string& packageName) override;
        void on_package_finish(const Package *package, const std::string& packageName) override;
        bool on_entry_start(const Entry *entry) override;
        void on_entry_finish(const Entry *entry) override;
        bool on_block_start(const Block *block) override;
        void on_block_finish(const Block *block) override;
    };

    class ExtractorWindow : public wxDialog {

        friend struct PackageSelector;

    protected:
        wxGauge* progressBar;
        wxDirPickerCtrl* dir;
        wxButton* mode_single;
        wxButton* mode_folder;
        wxTextCtrl* output;
        wxStaticLine* spacer;
        wxStaticText* m_staticText1;
        Callbacks callbacks;

        void load_package_single(wxCommandEvent& event);
        void load_package_folder(wxCommandEvent& event);
        void on_dir_changed(wxFileDirPickerEvent& event);
    public:
        ExtractorWindow();
        std::string get_folder();
        void add_output_line(std::string_view line);
        void clear_output();
        void init_progress(double d);
        void set_progress(double d);
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


