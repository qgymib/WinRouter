#ifndef WR_WIDGETS_INIT_HPP
#define WR_WIDGETS_INIT_HPP

#include <wx/wx.h>

namespace wr
{

/**
 * @brief Global unique ID.
 *
 * Format: `WR_${PANEL}_${TYPE}_${NAME}`
 */
enum WidgetID
{
    WIDGET_LOWEST = wxID_HIGHEST + 1,

    WIDGET_ADAPTER_LIST_BOX,

    WIDGET_DEBUG_BUTTON_COPY_TO_CLIPBOARD,
    WIDGET_DEBUG_BUTTON_OPEN_ONLINE_JSON_EDITOR,
    WIDGET_DEBUG_COMBOBOX_FUNC,

    WIDGET_MAIN_MENU_RUN_AS_ADMIN_MENU,
    WIDGET_MAIN_MENU_SHOW_LOG,
};

} // namespace wr

#endif // WR_WIDGETS_INIT_HPP
