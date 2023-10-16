#include "Weather.h"
#include "widgets/lv_arc.h"
#include "widgets/lv_img.h"

namespace domoticz {
namespace devices {

Weather::Weather(uint32_t ID, String unit, lv_obj_t *lv, lv_obj_t *lv2) : Device(ID, unit, lv, lv2) {}

Weather::~Weather() {}

bool Weather::update(JsonObject obj)
{
    const char *s = obj["svalue1"];
    if (s == NULL) {
        Logger::Error("error getting svalue1");
        return false;
    }
    String val = String(s);
    setLVText(lv_, val.c_str(), unit_.c_str());
    if (img_getter != NULL) {
        s = obj["svalue5"];
        if (s == NULL) {
            Logger::Error("error getting svalue5");
            return false;
        }

        val                     = String(s);
        uint32_t img_id         = (uint32_t)val.toInt();
        const lv_img_dsc_t *img = img_getter->getImage(img_id);
        if (img != NULL && lv2_ != NULL) {
            lv_img_set_src(lv2_, img);
        }
    }
    return true;
}

void Weather::get()
{
    String weather, temperature;
    if (WADomoticzGetWeatherAndTemperature(ID_, weather, temperature)) {
        setLVText(lv_, temperature.c_str(), unit_.c_str());
        if (img_getter != NULL) {
            const lv_img_dsc_t *img = img_getter->getImage(weather.toInt());
            if (img != NULL && lv2_ != NULL) {
                lv_img_set_src(lv2_, img);
            }
        }
    }
}

} // namespace devices
} // namespace domoticz