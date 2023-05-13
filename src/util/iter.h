#ifndef GUARD_ITER_H
#define GUARD_ITER_H

#include "state/exodus_state.h"

class Iterator {
    public:
        Iterator();
        // FIXME: Should really return Iterator&
        virtual void operator++();
        virtual bool complete();
    protected:
        int idx;
        int end;
};

class StarIterator : public Iterator {
    public:
        StarIterator();
        Star* get();
        int get_idx();
        Star* operator->() { return get(); }
    private:
        int n_stars;
        Star* s;
};

class PlanetIterator : public Iterator {
    public:
        PlanetIterator();
        PlanetIterator(int);
        virtual void operator++() override;
        virtual bool complete() override;
        Planet* get();
        Star* get_star();
        int get_idx();
        int get_star_idx();
        Planet* operator->() { return get(); }
    private:
        StarIterator star_iter;
        int owner;
        bool valid();
};

#endif
