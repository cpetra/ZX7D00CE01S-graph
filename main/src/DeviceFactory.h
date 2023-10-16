#ifndef DEVICEFACTORY_H
#define DEVICEFACTORY_H
#include "Device.h"
#include "ImageGetter.h"

using namespace domoticz::devices;


typedef struct temperature_item_ {
    uint32_t dmid;
    lv_obj_t *t;
} temperature_item;

typedef struct weather_item_ {
    uint32_t dmid;
    lv_obj_t *t;
    lv_obj_t *img;
} weather_item;

typedef struct weather_item_composed_ {
    weather_item w;
    temperature_item t;
} weather_item_composed;

typedef struct weather_config_ {
    temperature_item crt_t;
    weather_item_composed day0;
    weather_item_composed day1;
    weather_item_composed day2;
    weather_item_composed day3;
    weather_item_composed day4;
    lv_obj_t *day_arr[5];
} weather_config;

enum DEVICE_TYPE {
    TYPE_TEMPERATURE,
    TYPE_TEMPERATURE_HUMIDITY,
    TYPE_LIGHT,
    TYPE_POWER,
    TYPE_SWITCH,
    TYPE_THERMOSTAT,
    TYPE_SELECTOR,
    TYPE_WEATHER,
    // Add more sensor types as needed
};

class DeviceFactory
{
public:
    DeviceFactory();
    ~DeviceFactory();
    static Device *create(uint32_t id, DEVICE_TYPE type, String unit, const std::vector<lv_obj_t *>& lv, ImageGetter *img_getter, uint32_t mode = 0);
    void setImageGetter(ImageGetter *getter) { img_getter = getter; }

private:
    ImageGetter *img_getter;
};

#endif