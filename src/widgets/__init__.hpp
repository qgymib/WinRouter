#ifndef WR_WIDGETS_INIT_HPP
#define WR_WIDGETS_INIT_HPP

#include <wx/wx.h>

namespace wr
{

enum WidgetID
{
    WR_WIDGET_ID_NONE = wxID_HIGHEST + 1,

    WR_WIDGET_ADAPTER_LIST_BOX,
    WR_WIDGET_DEBUG_FUNC_COMBO,
    WR_WIDGET_DEBUG_REFRESH_BUTTON,
    WR_WIDGET_DEBUG_COPY_TO_CLIPBOARD_BUTTON,
    WR_WIDGET_DEBUG_OPEN_URL_BUTTON,
};

} // namespace wr

#endif // WR_WIDGETS_INIT_HPP
