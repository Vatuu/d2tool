#include "main.h"
#include "window_extractor.h"

namespace d2 {
    bool Destiny2Tool::OnInit() {
        auto *window = new ui::ExtractorWindows();
        window->Show(true);
        return true;
    }

    wxIMPLEMENT_APP(Destiny2Tool);
}