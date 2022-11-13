#ifndef GUARD_NEWSITEM_H
#define GUARD_NEWSITEM_H

#define MAX_NEWSITEMS 20

enum NewsItemType {
    NI_None,
    NI_Debug
};

class NewsItem {
    public:
        NewsItem();
        NewsItem(int, int, NewsItemType);
        const NewsItemType type;
        const int star_idx;
        const int planet_idx;
        const char* get_string() const;
};

#endif
