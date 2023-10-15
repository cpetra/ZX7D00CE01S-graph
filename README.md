# ZX7D00CE01S
ZX7D00CE01S base on https://github.com/espressif/esp-idf/tree/release/v5.0

#UI replace cmakefiles.txt with:
SET(SOURCES_DIRS "components" 
"fonts" 
"screens" 
"images" 
"." 
) 

idf_component_register(SRC_DIRS "${SOURCES_DIRS}" 
  INCLUDE_DIRS "${SOURCES_DIRS}" 
  REQUIRES lvgl 
)