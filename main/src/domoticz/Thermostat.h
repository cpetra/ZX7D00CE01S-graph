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

private:
    lv_obj_t *lv2_;
};

} // namespace devices
} // namespace domoticz

#endif