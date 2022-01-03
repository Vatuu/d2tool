//
// Created by Vatuu on 03/01/2022.
//

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/gauge.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/listbox.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/dialog.h>

namespace d2::ui {
    class ExtractorWindows : public wxDialog {

    protected:
        wxGauge* progress;
        wxButton* mode_single;
        wxButton* mode_folder;
        wxListBox* output;
        wxStaticLine* spacer;
        wxStaticText* m_staticText1;

        // Virtual event handlers, override them in your derived class
        virtual void load_package_single( wxCommandEvent& event ) { event.Skip(); }
        virtual void load_package_folder( wxCommandEvent& event ) { event.Skip(); }
    public:
        ExtractorWindows();
    };
}


