#include "DomoticzUI.h"
#include "Logger.h"
#include "WebApp.h"
#include "widgets/lv_label.h"


StaticJsonDocument<2000> doc;

DomoticzUI::DomoticzUI()
{
    img_getter   = NULL;
    current_day_ = -1;
}

DomoticzUI::~DomoticzUI()
{
    for(auto& pair : devs_) {
        delete pair.second;
    }
    devs_.clear();
}

void DomoticzUI::addDevice(Device *device)
{
    std::lock_guard<std::mutex> lock(mtx);
    devs_.insert(std::make_pair(device->getID(), device));
}

void DomoticzUI::addDevice(uint32_t id, DEVICE_TYPE type, String unit, const std::vector<lv_obj_t *>& vec)
{
    Device *dev = DeviceFactory::create(id, type, unit, vec, img_getter);
    addDevice(dev);
}

void DomoticzUI::addDevice(uint32_t id, DEVICE_TYPE type, String unit, const std::vector<lv_obj_t *>& vec, uint32_t mode)
{
    Device *dev = DeviceFactory::create(id, type, unit, vec, img_getter, mode);
    addDevice(dev);
}

void DomoticzUI::removeSensor(uint32_t id)
{
    std::lock_guard<std::mutex> lock(mtx);
    auto it      = devs_.find(id);
    delete it->second;
    devs_.erase(id);
}


void DomoticzUI::setDayWindows()
{
    static const String dayArr[] = {"Duminica", "Luni", "Marti", "Miercuri", "Joi", "Vineri", "Sambata", "Duminica"};
    uint32_t id;
    for (uint32_t i = 0; i < 5; i++) {
        id         = (current_day_ + i) % 7;
        String str = dayArr[id];
        for (auto &label : img_days[i]) {
            lv_label_set_text(label, str.c_str());
        }
    }
}

bool DomoticzUI::onMqttMesage(void *data, void *topic, void *payload)
{
    doc.clear();

    auto err = deserializeJson(doc, (const char *)payload);
    if (err != DeserializationError::Ok) {
        Logger::Error("Cannot deserialize json, error " + String(err.c_str()));
    }
    std::lock_guard<std::mutex> lock(mtx);
    uint32_t idx = (uint32_t)doc["idx"].as<int>();
    auto it      = devs_.find(idx);
    if (it != devs_.end()) {
        Logger::Info("MQTT message from sensor " + String(idx) + " " + String((char *)topic));
        Logger::Verbose((const char *)payload);

        Device *dev = it->second;
        JsonObject obj         = doc.as<JsonObject>();
        if (!dev->update(obj)) {
            Logger::Error("Cannot update obj");
            Logger::Error((char *)payload);
        }
        return true;
    }
    else {
        Logger::Debug("Got message from unregistered sensor " + String(idx) + " " + String((char *)topic));
    }
    // Get day of week
    String current_date = doc["LastUpdate"];
    if (current_date == "") {
        Logger::Error("Date receive error");
    }
    else {
        Logger::Verbose(current_date);
        int current_day = WAGetDayOfWeek(current_date);
        if (current_day != current_day_) {
            current_day_ = current_day;
            setDayWindows();
        }
    }
    Logger::Verbose("Message handled for " + String(idx));
    return false;
}

void DomoticzUI::setRangeValue(uint32_t id, float value, bool update)
{
    auto it = devs_.find(id);
    if (it != devs_.end()) {
        Device *dev = it->second;
        dev->setRange(value, update);
        return;
    }
}

void DomoticzUI::setRangeValuebyObj(lv_obj_t *obj, float value, bool update)
{
    auto it = devs_.begin();
    while (it != devs_.end()) {
        Device *dev = it->second;
        lv_obj_t *lv, *lv2;
        dev->getObjs(&lv, &lv2);
        if (lv2 == obj) {
            dev->setRange(value, update);
            return;
        }
        it++;
    }
}
bool DomoticzUI::updateSensor(uint32_t id)
{
    std::lock_guard<std::mutex> lock(mtx);
    auto it = devs_.find(id);
    if (it != devs_.end()) {
        Device *dev = it->second;
        if (dev != NULL) {
            dev->get();
        }
        else {
            Logger::Error("Updating null dev pointer!");
        }
        return true;
    }
    return false;
}

bool DomoticzUI::updateAll()
{
    std::lock_guard<std::mutex> lock(mtx);

    for (auto &pair : devs_) {
        Device *dev = pair.second;
        dev->get();
    }
    return true;
}

bool DomoticzUI::addWeather(weather_config *w)
{
    if (w == NULL) {
        return false;
    }
    addDevice(w->crt_t.dmid, TYPE_TEMPERATURE, "°C", {w->crt_t.t});
    addDevice(w->day0.w.dmid, TYPE_WEATHER, "", {w->day0.w.t, w->day0.w.img});
    addDevice(w->day0.t.dmid, TYPE_TEMPERATURE, "", {w->day0.t.t});
    addDevice(w->day1.w.dmid, TYPE_WEATHER, "", {w->day1.w.t, w->day1.w.img});
    addDevice(w->day1.t.dmid, TYPE_TEMPERATURE, "",  {w->day1.t.t});
    addDevice(w->day2.w.dmid, TYPE_WEATHER, "", {w->day2.w.t, w->day2.w.img});
    addDevice(w->day2.t.dmid, TYPE_TEMPERATURE, "", {w->day2.t.t});
    addDevice(w->day3.w.dmid, TYPE_WEATHER, "", {w->day3.w.t, w->day3.w.img});
    addDevice(w->day3.t.dmid, TYPE_TEMPERATURE, "", {w->day3.t.t});
    addDevice(w->day4.w.dmid, TYPE_WEATHER, "", {w->day4.w.t, w->day4.w.img});
    addDevice(w->day4.t.dmid, TYPE_TEMPERATURE, "", {w->day4.t.t});
    for (int i = 0; i < 5; i++) {
        img_days[i].push_back(w->day_arr[i]);
    }
    return true;
}
