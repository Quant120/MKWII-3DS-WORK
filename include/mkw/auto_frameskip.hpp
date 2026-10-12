#pragma once

namespace mkw {
// Simulation is always 60 Hz. Only suppress whole presentation slots.
struct AutoFrameskip {
    unsigned pressure=0,skipped=0,quiet=0;
    void reset(){pressure=skipped=quiet=0;}
    void observe(unsigned refreshes,double pending){
        if(refreshes>1||pending>=1.0){pressure=pressure+4>40?40:pressure+4;quiet=0;}
        else if(++quiet>=3){if(pressure)--pressure;quiet=0;}
    }
    bool drop(){
        const unsigned limit=pressure>=24?2u:pressure>=8?1u:0u;
        if(!limit){skipped=0;return false;}
        if(skipped<limit){++skipped;return true;}
        skipped=0;return false;
    }
};
}
