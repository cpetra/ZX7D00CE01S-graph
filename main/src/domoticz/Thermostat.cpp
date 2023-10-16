#include "Thermostat.h"
#include "Logger.h"
#include "widgets/lv_arc.h"

namespace domoticz {
namespace devices {

Thermostat::Thermostat(uint32_t ID, String unit, lv_obj_t *lv, lv_obj_t *lv2) : Device(ID, unit, lv, lv2)
{
}

Thermostat::~Thermostat() {}

bool Thermostat::update(JsonObject obj)
{
    const char *s = obj["svalue1"];
    if (s == NULL) {
        Logger::Error("got NULL value ");
        return false;
    }
    setLVText(lv_, s, unit_.c_str());
    return true;
}

void Thermostat::get() 
{
    String value = WADomoticzGetSetPoint(ID_);
    setLVText(lv_, value.c_str(), unit_.c_str());
    if (lv2_ != NULL) {
        Logger::Debug("ptr: " + String((uint32_t)lv2_));

        float fval = value.toFloat();
        uint32_t v = (int)fval * 2;
        int16_t vmin, vmax;
        vmin = lv_arc_get_min_value(lv2_);
        vmax = lv_arc_get_max_value(lv2_);
        Logger::Debug(String(vmin));
        Logger::Debug(String(vmax));
        if (v < vmin) {
            v = vmin;
        }
        if (v > vmax) {
            v = vmax;
        }

        lv_arc_set_value(lv2_, v);
    }
}

void Thermostat::setRange(float value, bool update)
{
    String temperature_val = String(value, 1);
    if (update) {
        WADomoticzSetThermostat(ID_, (float)value);
    }
    else {
        setLVText(lv_, temperature_val.c_str(), unit_.c_str());
    }
} 


} // namespace devices
} // namespace domoticz
