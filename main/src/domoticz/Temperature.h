#ifndef TEMPERATURE_H
#define TEMPERATURE_H
#include "Device.h"

namespace domoticz {
namespace devices {

class Temperature : public Device
{
public:
    Temperature(uint32_t ID, String unit, lv_obj_t *lv);
    ~Temperature();
    bool update(JsonObject obj) override;
    void get() override;

private:
};
} // namespace devices
} // namespace domoticz

#endif