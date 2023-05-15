#ifndef GUARD_ITER_H
#define GUARD_ITER_H

#include "state/exodus_state.h"

#include "exodus_features.h"

#if FEATURE_PLANET_ITERATION_RANDOMISE
#define STARITER StarIteratorRandom
#define PLANETITER PlanetIteratorRandom
#else
#define STARITER StarIterator
#define PLANETITER PlanetIterator
#endif

class Iterator {
    public:
        Iterator();
        // FIXME: Should really return Iterator&
        virtual void operator++();
        virtual bool complete();
        virtual void set_random();
    protected:
        void reset_idx();
        int get_effective_idx();
        int idx;
        int end;
        bool randomise;
    private:
        int* random_data;
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
    protected:
        StarIterator star_iter;
        bool valid();
    private:
        int owner;
};

class StarIteratorRandom : public StarIterator {
    public:
        StarIteratorRandom() : StarIterator() { set_random(); }
};

class PlanetIteratorRandom : public PlanetIterator {
    public:
        PlanetIteratorRandom() : PlanetIterator() { star_iter.set_random(); set_random(); }
        PlanetIteratorRandom(int x) : PlanetIterator(x) { star_iter.set_random(); set_random(); }
        virtual void set_random() override;
};

#endif
