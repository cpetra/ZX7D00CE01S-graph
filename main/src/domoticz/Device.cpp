#include "Device.h"
#include "Logger.h"
#include "widgets/lv_label.h"

namespace domoticz {
namespace devices {

void Device::setLVText(lv_obj_t *l, const char *text, const char *unit)
{
    if (l == NULL) {
        Logger::Error(String(__FUNCTION__) + String(": lv_obj is NULL"));
        return;
    }
    if (text != NULL) {
        String str = String(text);
        if (unit != NULL)
            str += String(unit);
        lv_label_set_text(l, str.c_str());
    }
}

Device::Device(uint32_t ID, String unit, lv_obj_t* lv, lv_obj_t* lv2)
{
    ID_ = ID;
    lv_ = lv;
    lv2_ = lv2;
    unit_ = unit;
    img_getter = NULL;
}
Device::~Device()
{

}

uint32_t Device::getID() const
{
    return ID_;
}

lv_obj_t *Device::getObj() const
{
    return lv_;
}
void Device::setRange(float value, bool update)
{
    Logger::Error("setRange called on object not implementing the functionality");
}
void Device::getObjs(lv_obj_t **lv1, lv_obj_t **lv2) const
{
    *lv1 = lv_;
    *lv2 = lv2_;       
}

}
}
