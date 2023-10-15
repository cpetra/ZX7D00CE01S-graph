#include "ImageGetter.h"

ImageGetter::ImageGetter()
{

}

ImageGetter::~ImageGetter()
{

}
void ImageGetter::addImage(uint32_t id, const lv_img_dsc_t *img)
{
    map_.insert(std::make_pair(id, img));
}
const lv_img_dsc_t * ImageGetter::getImage(uint32_t id)
{
    auto it = map_.find(id);
    if (it != map_.end()) {
        return (const lv_img_dsc_t *) it->second;
    }
    return NULL;
}
