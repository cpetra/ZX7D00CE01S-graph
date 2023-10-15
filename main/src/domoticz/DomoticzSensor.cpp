#include "DomoticzSensor.h"
#include "Logger.h"
#include "WebApp.h"
#include "core/lv_obj.h"
#include "widgets/lv_arc.h"
#include "widgets/lv_img.h"
#include "widgets/lv_label.h"
#include "widgets/lv_dropdown.h"
#include <Arduino.h>

void setLVText(lv_obj_t *l, const char *text, const char *unit)
{
    if (l == NULL) {
        Logger::Error(String(__FUNCTION__) + String(": lv_obj is NULL"));
        return;
    }
    if (text != NULL) {
        String str = String(text);
        if (unit != NULL)
            str += String(unit);
        lv_label_set_text(l, str.c_str());
    }
}

void setLVSelector(lv_obj_t *l, int id)
{
    if (l == NULL) {
        Logger::Error(String(__FUNCTION__) + String(": lv_obj is NULL"));
        return;
    }
    if (id >= 0) {
        Logger::Debug("LV Select: " + String(id));
        id = (id / 10); // convert from domoticz selector levels
        lv_dropdown_set_selected(l, id);
    }
}

int getPowerValue(String usage)
{
    String str = "";
    int count  = 0;
    int found  = 0;
    for (int i = 0; i < usage.length(); i++) {
        if (usage[i] == ';') {
            ++count;
            if (count == 4) {
                found = 1;
            }
            if (count == 5) {
                break;
            }
        }
        else {
            if (found) {
                str += usage[i];
            }
        }
    }
    int ival = str.toInt();
    return ival;
}

void setLVSwitch2(lv_obj_t *l, String state)
{
    if (state == "On") {
        lv_obj_add_state(l, LV_STATE_CHECKED);
    }
    else {
        lv_obj_clear_state(l, LV_STATE_CHECKED);
    }
}

void setLVSwitch(lv_obj_t *l, const char *topic, int state)
{
    if (strcmp(topic, "domoticz/out") != 0) {
        return;
    }
    if (state == 1) {
        lv_obj_add_state(l, LV_STATE_CHECKED);
    }
    else {
        lv_obj_clear_state(l, LV_STATE_CHECKED);
    }
}
void DomoticzSensor::getLvObjs(lv_obj_t **lv, lv_obj_t **lv2)
{
    *lv  = lv_;
    *lv2 = lv2_;
}

bool DomoticzSensor::update(JsonObject obj)
{
    String val;
    switch (type_) {
    case TYPE_TEMPERATURE_SELECT:
    case TYPE_TEMPERATURE: {
        const char *s = obj["svalue1"];
        if (s == NULL) {
            Logger::Error("got NULL value ");
            return false;
        }
        val = String(s);
        setLVText(lv_, s, unit_.c_str());
    } break;
    case TYPE_TEMPERATURE_HUMIDITY: {
        const char *s = obj["svalue1"];
        if (s == NULL) {
            Logger::Debug("got NULL value ");
            return false;
        }
        val = String(s);
        Logger::Debug("got svalue1 of " + val);
        setLVText(lv_, s, unit_.c_str());
    } break;
    case TYPE_SWITCH:
        // ToDo: topic needs to be domoticz/out
        // setLVSwitch(lv_, NULL, doc["nvalue"].as<int>());
        break;
    case TYPE_WEATHER:
        //
        //{ "Battery" : 255, "LastUpdate" : "2023-10-06 15:12:52", "RSSI" : 12, "description" : "", "dtype" : "Temp + Humidity +
        //Baro", "hwid" : "17", "id" : "0011", "idx" : 282, "name" : "TempHumBaro Floresti Day 0", "nvalue" : 0, "stype" :
        //"Weather Station", "svalue1" : "18.6", "svalue2" : "43", "svalue3" : "1", "svalue4" : "1025.0", "svalue5" : "5", "unit"
        //: 17 }
        {
            const char *s = obj["svalue1"];
            if (s == NULL) {
                Logger::Error("error getting svalue1");
                break;
            }
            val = String(s);
            setLVText(lv_, val.c_str(), unit_.c_str());
            if (img_getter != NULL) {
                s = obj["svalue5"];
                if (s == NULL) {
                    Logger::Error("error getting svalue5");
                    break;
                }

                val                     = String(s);
                uint32_t img_id         = (uint32_t)val.toInt();
                const lv_img_dsc_t *img = img_getter->getImage(img_id);
                if (img != NULL && lv2_ != NULL) {
                    lv_img_set_src(lv2_, img);
                }
            }
        }
        break;
    case TYPE_SELECTOR: {
            int level = obj["Level"].as<int>();
            setLVSelector(lv_, level);
        }

        break;
    case TYPE_THERMOSTAT_STATE: {
        String str = obj["Data"].as<String>();
        if (str == "0%") {
            color_ = NULL;
        }
        else {

        }
    }
        break;
    default:
        Logger::Error("Unknown sensor type");
        return false;
        break;
        // Add more sensor t
    }
    return true;
}

void DomoticzSensor::setRangeValue(float value, bool update)
{
    String temperature_val = String(value, 1);
    if (update) {
        WADomoticzSetThermostat(ID_, (float)value);
    }
    else {
        setLVText(lv_, temperature_val.c_str(), unit_.c_str());
    }
}

void DomoticzSensor::get()
{
    switch (type_) {
    case TYPE_TEMPERATURE_SELECT: {
        String value = WADomoticzGetSetPoint(ID_);
        setLVText(lv_, value.c_str(), unit_.c_str());
        if (lv2_ != NULL) {

            float fval = value.toFloat();
            uint32_t v = (int)fval * 2;
            int16_t vmin, vmax;
            vmin = lv_arc_get_min_value(lv2_);
            vmax = lv_arc_get_max_value(lv2_);
            if (v < vmin) {
                v = vmin;
            }
            if (v > vmax) {
                v = vmax;
            }

            lv_arc_set_value(lv2_, v);
        }
    } break;
    case TYPE_WEATHER: {
        String weather, temperature;
        if (WADomoticzGetWeatherAndTemperature(ID_, weather, temperature)) {
            setLVText(lv_, temperature.c_str(), unit_.c_str());
            if (img_getter != NULL) {
                const lv_img_dsc_t *img = img_getter->getImage(weather.toInt());
                if (img != NULL && lv2_ != NULL) {
                    lv_img_set_src(lv2_, img);
                }
            }
        }
    } break;
    case TYPE_TEMPERATURE:
    case TYPE_TEMPERATURE_HUMIDITY:
        setLVText(lv_, WADomoticzGetTemperature(ID_).c_str(), unit_.c_str());
        break;
    case TYPE_SWITCH:
        setLVSwitch2(lv_, WADomoticzGetSwitchStatus(ID_));
        break;
    case TYPE_SELECTOR:
        setLVSelector(lv_, WADomoticzGetSelector(ID_));
        break;
    default:
        break;
    }
}
