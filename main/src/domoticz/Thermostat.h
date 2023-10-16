#ifndef THERMOSTAT_H
#define THERMOSTAT_H
#include "Device.h"

namespace domoticz {
namespace devices {

class Thermostat : public Device
{
public:
    Thermostat(uint32_t ID, String unit, lv_obj_t *lv, lv_obj_t *lv2);
    ~Thermostat() override;
    bool update(JsonObject obj) override;
    void get() override;
    void setRange(float value, bool update); 
};

} // namespace devices
} // namespace domoticz

#endif