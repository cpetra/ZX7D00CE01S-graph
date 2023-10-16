#include "DeviceFactory.h"
#include "Temperature.h"
#include "Thermostat.h"
#include "Logger.h"
#include "Selector.h"
#include "Weather.h"
#include "ImageGetter.h"

DeviceFactory::DeviceFactory() {}

DeviceFactory::~DeviceFactory() {}
Device *DeviceFactory::create(uint32_t id, DEVICE_TYPE type, String unit, const std::vector<lv_obj_t *>& lv, ImageGetter *img_getter, uint32_t mode)
{
    Device *dev = NULL;
    if (lv.size() < 1) {
        Logger::Error("Device Factory Error: no lv_obj_t * passed");
        return NULL;
    }

    switch (type) {
    case TYPE_TEMPERATURE:
        dev = new Temperature(id, unit, lv[0]);
        break;
    case TYPE_THERMOSTAT:
        if (lv.size() < 2) {
            Logger::Error("Device Factory Error: Thermostat type needs two lv_obj_t *");
            return NULL;
        }
        Logger::Debug("ptr2: " + String((uint32_t)lv[1]));

        dev = new Thermostat(id, unit, lv[0], lv[1]);
        break;
    case TYPE_SELECTOR:
        dev = new Selector(id, unit, lv[0], mode);
        break;
    case TYPE_WEATHER:
        dev = new Weather(id, unit, lv[0], lv[1]);
        if (dev != NULL) {
            dev->setImageGetter(img_getter);
        }
        break;
    default:
        Logger::Error("Device Factory Error: Unknown type" + String(type));
        return NULL;
    }
    return dev;
}
