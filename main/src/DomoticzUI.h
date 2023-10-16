#ifndef DOMOTICZUI_H
#define DOMOTICZUI_H

#include "ImageGetter.h"
#include <map>
#include <mutex>
#include <vector>
#include "Device.h"
#include "DeviceFactory.h"

using namespace domoticz::devices;

class DomoticzUI
{
public:
    DomoticzUI();
    ~DomoticzUI();
    void addDevice(uint32_t id, DEVICE_TYPE type, String unit, const std::vector<lv_obj_t *>& vec);
    void addDevice(uint32_t id, DEVICE_TYPE type, String unit, const std::vector<lv_obj_t *>& vec, uint32_t mode);
    void addDevice(Device *device);

    void removeSensor(uint32_t id);
    bool addWeather(weather_config *w);


    bool onMqttMesage(void *data, void *topic, void *payload);
    void setRangeValue(uint32_t id, float value, bool update = false);
    void setRangeValuebyObj(lv_obj_t *obj, float value, bool update = false);
    bool updateSensor(uint32_t id);
    bool updateAll();
    void setImageGetter(ImageGetter *getter) { img_getter = getter; }


private:
    void setDayWindows(); 
    std::map<uint32_t, Device *> devs_;
    ImageGetter *img_getter;
    std::vector<lv_obj_t *> img_days[5];
    std::mutex mtx;
    int current_day_;
    lv_color_t *color_;
};

#endif