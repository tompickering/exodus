#ifndef GUARD_EXODUS_H
#define GUARD_EXODUS_H

class Exodus {
    public:
        Exodus();
        ~Exodus();
        int run(int argc, char** argv);
    private:
        bool init();
};

#endif
