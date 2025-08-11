#ifndef GUARD_SAVE_LINUX_H
#define GUARD_SAVE_LINUX_H

#include "save.h"

class SaveManagerLinux : public SaveManager {
    protected:
        virtual const char* get_save_dir() override;
};

#endif
