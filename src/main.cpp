#include "main.h"

namespace d2 {

    bool Destiny2Tool::OnInit() {
        auto *window = new ui::ExtractorWindow();
        window->Show(true);
        window->add_output_line("Please make a selection...");
        return true;
    }

    wxIMPLEMENT_APP(Destiny2Tool);
}