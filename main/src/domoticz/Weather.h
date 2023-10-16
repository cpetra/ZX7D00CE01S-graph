#ifndef WEATHER_H
#define WEATHER_H

#include "Device.h"

namespace domoticz {
namespace devices {

class Weather : public Device
{
public:
    Weather(uint32_t ID, String unit, lv_obj_t *lv, lv_obj_t *lv2);
    ~Weather() override;
    bool update(JsonObject obj) override;
    void get() override;
};

} // namespace devices
} // namespace domoticz

#endif