#include "Selector.h"
#include "Logger.h"
#include "widgets/lv_dropdown.h"

namespace domoticz {
namespace devices {

Selector::Selector(uint32_t ID, String unit, lv_obj_t *lv) : Device(ID, unit, lv) {}
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
    int level = obj["Level"].as<int>();
    setUI(lv_, level);
    return true;
}

void Selector::get()  { setUI(lv_, WADomoticzGetSelector(ID_));}

} // namespace devices
} // namespace domoticz
