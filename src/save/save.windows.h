#ifndef GUARD_SAVE_WINDOWS_H
#define GUARD_SAVE_WINDOWS_H

#include "save.h"

class SaveManagerWindows : public SaveManager {
    protected:
        virtual const char* get_save_dir() override;
};

#endif
