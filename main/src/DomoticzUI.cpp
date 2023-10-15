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

void DomoticzUI::addSensor(uint32_t id, DomoticzSensor::DOMOTICZ_TYPE type, lv_obj_t *lv, String unit, lv_obj_t *graph)
{
    if (id == 0 || lv == NULL) {
        Logger::Error("Failed to add sensors with id: " + String(id) + " and lv_obj_t : " + String((uint32_t)lv));
        return;
    }
    DomoticzSensor sensor(id, type, lv, unit, graph);
    std::lock_guard<std::mutex> lock(mtx);
    map_.insert(std::make_pair(id, sensor));
}

void DomoticzUI::addSensor(uint32_t id, DomoticzSensor::DOMOTICZ_TYPE type, lv_obj_t *lv, lv_obj_t *lv2, String unit,
                           lv_obj_t *graph)
{
    if (id == 0 || lv == NULL) {
        Logger::Error("Failed to add sensors with id: " + String(id) + " and lv_obj_t : " + String((uint32_t)lv) +
                      " and lv_obj_t2 : " + String((uint32_t)lv2));
        return;
    }
    DomoticzSensor sensor(id, type, lv, lv2, unit, graph);
    std::lock_guard<std::mutex> lock(mtx);

    if (type == DomoticzSensor::TYPE_WEATHER) {
        sensor.setImageGetter(img_getter);
    }
    map_.insert(std::make_pair(id, sensor));
}

void DomoticzUI::addSensor(Device *device)
{
    std::lock_guard<std::mutex> lock(mtx);
    devs_.insert(std::make_pair(device->getID(), device));
}

void DomoticzUI::addSensor(uint32_t id, DEVICE_TYPE type, String unit, const std::vector<lv_obj_t *>& vec)
{
    Device *dev = DeviceFactory::create(id, type, unit, vec, img_getter);
    addSensor(dev);
}

void DomoticzUI::removeSensor(uint32_t id)
{
    std::lock_guard<std::mutex> lock(mtx);
    map_.erase(id);
}

void DomoticzUI::removeAllSensors()
{
        std::lock_guard<std::mutex> lock(mtx);
        map_.clear();
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
    auto it      = map_.find(idx);
    if (it != map_.end()) {
        Logger::Info("MQTT message from sensor " + String(idx) + " " + String((char *)topic));
        Logger::Verbose((const char *)payload);

        DomoticzSensor &sensor = it->second;
        JsonObject obj         = doc.as<JsonObject>();
        if (!sensor.update(obj)) {
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
    Logger::Debug(current_date);
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
    Logger::Debug("Message handled for " + String(idx));
    return false;
}

void DomoticzUI::setRangeValue(uint32_t id, float value, bool update)
{
    auto it = map_.find(id);
    if (it != map_.end()) {
        DomoticzSensor &sensor = it->second;
        sensor.setRangeValue(value, update);
        return;
    }
}

void DomoticzUI::setRangeValuebyObj(lv_obj_t *obj, float value, bool update)
{
    auto it = map_.begin();
    while (it != map_.end()) {
        DomoticzSensor &sensor = it->second;
        lv_obj_t *lv, *lv2;
        sensor.getLvObjs(&lv, &lv2);
        if (lv2 == obj) {
            sensor.setRangeValue(value, update);
            return;
        }

        it++;
    }
}
bool DomoticzUI::updateSensor(uint32_t id)
{
    std::lock_guard<std::mutex> lock(mtx);
    auto it = map_.find(id);
    if (it != map_.end()) {
        DomoticzSensor &sensor = it->second;
        sensor.get();
        return true;
    }
    return false;
}

bool DomoticzUI::updateAll()
{
    std::lock_guard<std::mutex> lock(mtx);

    for (auto &pair : map_) {
        DomoticzSensor &sensor = pair.second;
        sensor.get();
    }
    return true;
}

bool DomoticzUI::addWeather(weather_config *w)
{
    if (w == NULL) {
        return false;
    }
    addSensor(w->crt_t.dmid, DomoticzSensor::TYPE_TEMPERATURE, w->crt_t.t, "°C", NULL);
    addSensor(w->day0.w.dmid, DomoticzSensor::TYPE_WEATHER, w->day0.w.t, w->day0.w.img, "", NULL);
    addSensor(w->day0.t.dmid, DomoticzSensor::TYPE_TEMPERATURE, w->day0.t.t, "", NULL);
    addSensor(w->day1.w.dmid, DomoticzSensor::TYPE_WEATHER, w->day1.w.t, w->day1.w.img, "", NULL);
    addSensor(w->day1.t.dmid, DomoticzSensor::TYPE_TEMPERATURE, w->day1.t.t, "", NULL);
    addSensor(w->day2.w.dmid, DomoticzSensor::TYPE_WEATHER, w->day2.w.t, w->day2.w.img, "", NULL);
    addSensor(w->day2.t.dmid, DomoticzSensor::TYPE_TEMPERATURE, w->day2.t.t, "", NULL);
    addSensor(w->day3.w.dmid, DomoticzSensor::TYPE_WEATHER, w->day3.w.t, w->day3.w.img, "", NULL);
    addSensor(w->day3.t.dmid, DomoticzSensor::TYPE_TEMPERATURE, w->day3.t.t, "", NULL);
    addSensor(w->day4.w.dmid, DomoticzSensor::TYPE_WEATHER, w->day4.w.t, w->day4.w.img, "", NULL);
    addSensor(w->day4.t.dmid, DomoticzSensor::TYPE_TEMPERATURE, w->day4.t.t, "", NULL);
    for (int i = 0; i < 5; i++) {
        img_days[i].push_back(w->day_arr[i]);
    }
    return true;
}
