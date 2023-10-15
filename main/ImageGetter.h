#ifndef IMAGEGETTER_H
#define IMAGEGETTER_H
#include <map>
#include "draw/lv_img_buf.h"

class ImageGetter
{
public:
    ImageGetter();
    ~ImageGetter();
    void addImage(uint32_t id, const lv_img_dsc_t *img);
    const lv_img_dsc_t * getImage(uint32_t id);

private:
    std::map<uint32_t, const lv_img_dsc_t *> map_;
};

#endif