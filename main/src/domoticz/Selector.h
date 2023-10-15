#ifndef SELECTOR_H
#define SELECTOR_H

#include "Device.h"

namespace domoticz {
namespace devices {

class Selector : public Device
{
public:
    Selector(uint32_t ID, String unit, lv_obj_t *lv);
    ~Selector();
    bool update(JsonObject obj) override;
    void get() override;

private:
    void setUI(lv_obj_t *l, int id);
};
} // namespace devices
} // namespace domoticz

#endif