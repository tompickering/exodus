#ifndef GUARD_SAVE_LINUX_H
#define GUARD_SAVE_LINUX_H

#include "save.h"

class SaveManagerLinux : public SaveManager {
    protected:
        virtual bool save_data(uint32_t, int, const char*, const char*) override;
        virtual bool load_data(uint32_t, int, char**, char**) override;
        virtual const char* get_save_dir() override;
};

#endif
