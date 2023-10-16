#include "Selector.h"
#include "Logger.h"
#include "widgets/lv_dropdown.h"

namespace domoticz {
namespace devices {

Selector::Selector(uint32_t ID, String unit, lv_obj_t *lv, uint32_t mode) : Device(ID, unit, lv, NULL) {
    mode_ = mode;
    if (mode == MODE_COLOR) {
        color_ = lv_obj_get_style_text_color(lv, LV_PART_MAIN | LV_STATE_DEFAULT);
    }

}
Selector::~Selector() {}

void Selector::setUI(lv_obj_t *l, int id)
{
    if (l == NULL) {
        Logger::Error(String(__FUNCTION__) + String(": lv_obj is NULL"));
        return;
    }
    if (id >= 0) {
        Logger::Debug("LV Select: " + String(id));
        id = (id / 10); // convert from domoticz selector levels
        lv_dropdown_set_selected(l, id);
    }
}

bool Selector::update(JsonObject obj) 
{
    int level = obj["svalue1"].as<int>();
    if (mode_ == MODE_SELECTOR) {
        setUI(lv_, level);
    }
    else {
        // String jsonString;
        // serializeJson(obj, jsonString);
        // Logger::Error(jsonString);
        setColor(level);
    }
    return true;
}

void Selector::setColor(int level)
{
    if (level == 0) {
         lv_obj_set_style_text_color(lv_, color_, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    else {
         lv_obj_set_style_text_color(lv_, lv_color_hex(0xffff00), LV_PART_MAIN | LV_STATE_DEFAULT);
    }

}
void Selector::get()  {
    int selection = WADomoticzGetSelector((ID_));
    Logger::Debug("selection: " + String((uint32_t)selection));
    if (mode_ == MODE_SELECTOR) {
        setUI(lv_, selection);
    }
    else {
        setColor(selection);
    }
}

} // namespace devices
} // namespace domoticz
