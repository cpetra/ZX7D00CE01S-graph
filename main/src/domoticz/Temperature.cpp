#include "Temperature.h"
#include "Logger.h"
namespace domoticz {
namespace devices {

Temperature::Temperature(uint32_t ID, String unit, lv_obj_t *lv) : Device(ID, unit, lv) {}
Temperature::~Temperature() {}

bool Temperature::update(JsonObject obj) 
{
    const char *s = obj["svalue1"];
    if (s == NULL) {
        Logger::Error("got NULL value ");
        return false;
    }
    setLVText(lv_, s, unit_.c_str());
    return true;
}

void Temperature::get()  { setLVText(lv_, WADomoticzGetTemperature(ID_).c_str(), unit_.c_str()); }

} // namespace devices
} // namespace domoticz
