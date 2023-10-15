
#ifndef DOMOTICZ_UI_INCLUCED
#define DOMOTICZ_UI_INCLUCED

#include "core/lv_obj.h"
#include "ImageGetter.h"
#include "ArduinoJson.h"

void setLVText(lv_obj_t *l, const char *text, const char *unit);
void setLVSwitch(lv_obj_t *l, const char *topic, int state);
void setLVSwitch2(lv_obj_t *l, String state);
int getPowerValue(String usage);

typedef lv_img_dsc_t *(*IMGGETTER)(int ID); 

class DomoticzSensor {
public:
    enum DOMOTICZ_TYPE {
        TYPE_TEMPERATURE,
        TYPE_TEMPERATURE_HUMIDITY,
        TYPE_LIGHT,
        TYPE_POWER,
        TYPE_SWITCH,
        TYPE_TEMPERATURE_SELECT,
        TYPE_SELECTOR,
        TYPE_WEATHER,
        TYPE_THERMOSTAT_STATE,

        // Add more sensor types as needed
    };
    DomoticzSensor(uint32_t ID, DOMOTICZ_TYPE type, lv_obj_t* lv, String unit, lv_obj_t* graph)
        : ID_(ID), type_(type), lv_(lv), lv2_(0), unit_(unit), graph_(graph), img_getter(NULL), color_(NULL){}

    DomoticzSensor(uint32_t ID, DOMOTICZ_TYPE type, lv_obj_t* lv, lv_obj_t* lv2, String unit, lv_obj_t* graph)
        : ID_(ID), type_(type), lv_(lv), lv2_(lv2), unit_(unit), graph_(graph),  img_getter(NULL), color_(NULL){}
    void getLvObjs(lv_obj_t **lv, lv_obj_t **lv2);
    bool update(JsonObject obj);
    void setRangeValue(float value, bool update = false);
    void setImageGetter(ImageGetter *getter) {img_getter = getter;}
    void get();

private:

    uint32_t ID_;
    DOMOTICZ_TYPE type_;
    lv_obj_t* lv_;
    lv_obj_t* lv2_;
    String unit_;
    lv_obj_t* graph_;
    ImageGetter *img_getter;
    lv_color_t *color_;

};
#endif // DOMOTICZ_UI_INCLUCED