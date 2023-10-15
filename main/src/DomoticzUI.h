#ifndef DOMOTICZUI_H
#define DOMOTICZUI_H

#include "DomoticzSensor.h"
#include "ImageGetter.h"
#include <map>
#include <mutex>
#include <vector>
#include "Device.h"
#include "DeviceFactory.h"

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

class DomoticzUI
{
public:
    DomoticzUI();
    ~DomoticzUI();
    void addSensor(uint32_t id, DomoticzSensor::DOMOTICZ_TYPE, lv_obj_t *lv, String unit, lv_obj_t *graph);
    void addSensor(uint32_t id, DomoticzSensor::DOMOTICZ_TYPE, lv_obj_t *lv, lv_obj_t *lv2, String unit, lv_obj_t *graph);
    void addSensor(uint32_t id, DEVICE_TYPE type, String unit, const std::vector<lv_obj_t *>& vec);
    void addSensor(Device *device);

    void removeSensor(uint32_t id);
    bool addWeather(weather_config *w);


    bool onMqttMesage(void *data, void *topic, void *payload);
    void setRangeValue(uint32_t id, float value, bool update = false);
    void setRangeValuebyObj(lv_obj_t *obj, float value, bool update = false);
    bool updateSensor(uint32_t id);
    bool updateAll();
    void setImageGetter(ImageGetter *getter) { img_getter = getter; }
    void removeAllSensors();

private:
    void setDayWindows(); 
    std::map<uint32_t, DomoticzSensor> map_;
    std::map<uint32_t, Device *> devs_;
    ImageGetter *img_getter;
    std::vector<lv_obj_t *> img_days[5];
    std::mutex mtx;
    int current_day_;
    lv_color_t *color_;
};

#endif