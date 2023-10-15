#ifndef DEVICEFACTORY_H
#define DEVICEFACTORY_H
#include "Device.h"
#include "ImageGetter.h"

using namespace domoticz::devices;

enum DEVICE_TYPE {
    TYPE_TEMPERATURE,
    TYPE_TEMPERATURE_HUMIDITY,
    TYPE_LIGHT,
    TYPE_POWER,
    TYPE_SWITCH,
    TYPE_THERMOSTAT,
    TYPE_SELECTOR,
    TYPE_WEATHER,
    TYPE_THERMOSTAT_STATE,

    // Add more sensor types as needed
};

class DeviceFactory
{
public:
    DeviceFactory();
    ~DeviceFactory();
    static Device *create(uint32_t id, DEVICE_TYPE type, String unit, const std::vector<lv_obj_t *>& lv, ImageGetter *img_getter);
    void setImageGetter(ImageGetter *getter) { img_getter = getter; }

private:
    ImageGetter *img_getter;
};

#endif