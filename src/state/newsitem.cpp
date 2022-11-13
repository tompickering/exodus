#include "newsitem.h"

NewsItem::NewsItem()
    : star_idx(-1), planet_idx(-1), type(NI_None) {}

NewsItem::NewsItem(int star, int planet, NewsItemType _type)
    : star_idx(star), planet_idx(planet), type(_type) {}

const char* NewsItem::get_string() const {
    switch (type) {
        case NI_None:
            return "";
        case NI_Debug:
            return "TEST NEWS ITEM";
    }

    return "";
}
