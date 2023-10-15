#ifndef DEVICE_H_INCLUDED
#define DEVICE_H_INCLUDED
#include "core/lv_obj.h"
#include "ArduinoJson.h"
#include "WebApp.h"
#include "ImageGetter.h"

namespace domoticz {
    namespace devices {
        class Device {
        protected:

            uint32_t ID_;
            lv_obj_t * lv_;
            String unit_;
            void setLVText(lv_obj_t *l, const char *text, const char *unit);
            ImageGetter *img_getter;
        public:
            Device(uint32_t ID, String unit, lv_obj_t* lv);
            virtual ~Device();
            virtual bool update(JsonObject obj) = 0;
            virtual void get() = 0;
            uint32_t getID() const;
            lv_obj_t *getObj() const;
            void setImageGetter(ImageGetter *getter) {img_getter = getter;}
        };

        // ... other sensor-related classes and functionalities
    }
}
#endif // #define DEVICE_H_INCLUDED
