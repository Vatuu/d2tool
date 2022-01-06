#include "window.h"
#include "file_helper.h"
#include "extractor.h"

namespace d2::ui {

    void ExtractorWindows::load_package_single(wxCommandEvent &event) {
        if(dir->GetDirName() != wxEmptyString) {
            auto *selector = new PackageSelector((wxWindow *) this, dir->GetDirName());
            std::string path = dir->GetDirName().GetPath().utf8_string();
            std::vector<std::string> files = d2::files::dir_unique_packages(path, ".pkg");
            std::for_each(files.cbegin(), files.cend(), [&](const std::string &entry) { selector->add_name(wxString(entry)); });
            selector->Show(true);
        } else {
            clear_output();
            add_output_line("A package directory needs to be specified first.");
        }
    }

    void ExtractorWindows::load_package_folder(wxCommandEvent &event) {

    }

    void ExtractorWindows::on_dir_changed(wxFileDirPickerEvent &event) {
        std::string path = event.GetPath().utf8_string();
        if(!d2::files::dir_contains_extension(path, ".pkg")) {
            clear_output();
            if(d2::files::dir_is_empty(path))
                add_output_line("Selected directory is empty.");
            else
                add_output_line("Selected directory contains no packages.");
            dir->SetDirName(wxFileName());
        }
    }

    void PackageSelector::on_select(wxCommandEvent &event) {
        auto *file = new wxDirDialog(this, "Select a output directory...");
        if(file->ShowModal() == wxID_CANCEL) {
            ((ExtractorWindows*)m_parent)->clear_output();
            ((ExtractorWindows*)m_parent)->add_output_line("Extraction cancelled.");
            Close();
        }
        Close();
        std::string target = file->GetPath().utf8_string();
        wxArrayInt selections;
        list->GetSelections(selections);
        std::vector<std::string> packages;
        std::for_each(selections.begin(), selections.end(), [&](const int entry) { packages.push_back(list->GetString(entry).utf8_string()); });

        try {
            Extractor extractor(packages, ((ExtractorWindows*)m_parent)->get_folder(), target, (ExtractorWindows*)m_parent);
            extractor.extract();
        } catch(std::runtime_error& err) {
            ((ExtractorWindows*)m_parent)->clear_output();
            ((ExtractorWindows*)m_parent)->add_output_line("Unable to set up extractor!");
            ((ExtractorWindows*)m_parent)->add_output_line(err.what());
        }
    }

    void PackageSelector::on_cancel(wxCommandEvent &event) {
        ((ExtractorWindows*)m_parent)->clear_output();
        ((ExtractorWindows*)m_parent)->add_output_line("Extraction cancelled.");
        Close();
    }

    void PackageSelector::on_list(wxCommandEvent& event) {
        wxArrayInt meh;
        select->Enable(list->GetSelections(meh) > 0);
    }
};