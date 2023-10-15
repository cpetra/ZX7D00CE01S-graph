#include "Weather.h"
#include "widgets/lv_arc.h"
#include "widgets/lv_img.h"

namespace domoticz {
namespace devices {

Weather::Weather(uint32_t ID, String unit, lv_obj_t *lv, lv_obj_t *lv2) : Device(ID, unit, lv) { lv2_ = lv2; }

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
    String value = WADomoticzGetSetPoint(ID_);
    setLVText(lv_, value.c_str(), unit_.c_str());
    if (lv2_ != NULL) {

        float fval = value.toFloat();
        uint32_t v = (int)fval * 2;
        int16_t vmin, vmax;
        vmin = lv_arc_get_min_value(lv2_);
        vmax = lv_arc_get_max_value(lv2_);
        if (v < vmin) {
            v = vmin;
        }
        if (v > vmax) {
            v = vmax;
        }

        lv_arc_set_value(lv2_, v);
    }
}

} // namespace devices
} // namespace domoticz