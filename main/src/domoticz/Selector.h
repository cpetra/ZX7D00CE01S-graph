#ifndef SELECTOR_H
#define SELECTOR_H

#include "Device.h"

namespace domoticz {
namespace devices {

#define MODE_SELECTOR 0
#define MODE_COLOR  1
class Selector : public Device
{
public:
    Selector(uint32_t ID, String unit, lv_obj_t *lv, uint32_t mode = MODE_SELECTOR);
    ~Selector();
    bool update(JsonObject obj) override;
    void get() override;
private:
    void setUI(lv_obj_t *l, int id);
    void setColor(int ID);
    uint32_t mode_;
    lv_color_t color_;
};
} // namespace devices
} // namespace domoticz

#endif