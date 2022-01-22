#include "window.h"
#include "parse_helper.h"

namespace d2::ui {

    ExtractorWindow::ExtractorWindow() : wxDialog(nullptr, wxID_ANY, wxT("Destiny Tool"), wxDefaultPosition, wxSize(280, -1), wxDEFAULT_DIALOG_STYLE), callbacks(this) {
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

        mode_single->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ExtractorWindow::load_package_single), nullptr, this);
        mode_folder->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ExtractorWindow::load_package_folder), nullptr, this);
        dir->Connect(wxEVT_DIRPICKER_CHANGED, wxFileDirPickerEventHandler(ExtractorWindow::on_dir_changed), nullptr, this);
    }

    void ExtractorWindow::add_output_line(std::string_view line) {
        output->AppendText(wxString(std::string(line) + "\n"));
    }

    void ExtractorWindow::clear_output() {
        output->Clear();
    }

    std::string ExtractorWindow::get_folder() {
        return dir->GetPath().utf8_string();
    }

    Callbacks::Callbacks(ExtractorWindow *window) {
        this->window = window;
    }

    void Callbacks::on_error(const d2::extract::Error errorType, const std::variant<Package*, Entry*, Block*, std::monostate> object, std::optional<std::string> additionalInfo) {
        switch(errorType) {
            case d2::extract::Error::DECRYPT_FAILED: {
                Block *b = std::get<Block*>(object);
                add_line(parsing::format("!-Failed to decrypt Block #%d:\n!--%s", b->id, additionalInfo->c_str()));
                break;
            }
            case d2::extract::Error::OODLE_SETUP_FAILED: {
                add_line("! Failed to set up Oodle! Stopping.");
                break;
            }
            case d2::extract::Error::OODLE_DECOMPRESS_FAILED: { break; }
            case d2::extract::Error::MISSING_PATCH: {
                Block *b = std::get<Block*>(object);
                add_line(parsing::format("!-Missing Patch #%d for Block #%d!", b->patchId, b->id));
                break;
            }
        }
    }

    void Callbacks::log(const std::string& line) {
        add_line(line);
    }

    void Callbacks::on_extract_start() {
        //TODO Yes
    }

    void Callbacks::on_extract_end(bool errors) {
        add_line("----------------------");
        if(errors)
            add_line("Errors occurred during extraction!\nResulting data is probably invalid!");
        else
            add_line("Extraction complete.");
    }

    bool Callbacks::on_package_start(const Package *package, const std::string& packageName) {
        add_line(parsing::format("Extracting %s... (%d Patches)", packageName.c_str(), package->patches.size()));
        add_line(parsing::format("Entry Count: %d", package->entryTable.size()));
        return true;
    }

    void Callbacks::on_package_finish(const Package *package, const std::string& packageName) {
        add_line("Package complete.");
    }

    bool Callbacks::on_entry_start(const Entry *entry) {
        if(entry->fileSize <= 0) {
            add_line(parsing::format("-Entry #%d is empty, skipping.", entry->id));
            return false;
        }
        add_line(parsing::format("-Extracting Entry #%d...", entry->id));
        add_line(parsing::format("-Name: %s", entry->fileName.c_str()));
        add_line(parsing::format("-Size: %d Bytes", entry->fileSize));
        return true;
    }

    void Callbacks::on_entry_finish(const Entry *entry) {
        add_line("-Entry complete.");
    }

    bool Callbacks::on_block_start(const Block *block) {
        add_line(parsing::format("--Extracting Block #%d from Patch %d...", block->id, block->patchId));
        return true;
    }

    void Callbacks::on_block_finish(const Block *block) {
        add_line("--Block complete.");
    }

    void Callbacks::add_line(const std::string& line) {
        wxApp::GetMainTopWindow()->CallAfter([this, line]() {
            window->add_output_line(line);
        });
    }

    void Callbacks::clear() {
        wxApp::GetMainTopWindow()->CallAfter([this]() {
            window->clear_output();
        });
    }
}
