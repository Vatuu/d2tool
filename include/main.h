#pragma once

#include "window.h"
#include <wx/app.h>

namespace d2 {
    class Destiny2Tool : public wxApp {
    public:
        bool OnInit() override;
    };
}

