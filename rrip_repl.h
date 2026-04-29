#ifndef RRIP_REPL_H_
#define RRIP_REPL_H_

#include "repl_policies.h"

class SRRIPReplPolicy : public ReplPolicy {
private:
    uint32_t* rrpv;       // RRPV array per cache line
    uint32_t numLines;    // total number of cache lines
    uint32_t rpvMax;      // max RRPV value (e.g., 3 for 2-bit version)

public:
    explicit SRRIPReplPolicy(uint32_t _numLines, uint32_t _rpvMax)
        : numLines(_numLines), rpvMax(_rpvMax) {
        rrpv = gm_calloc<uint32_t>(numLines);
        for (uint32_t i = 0; i < numLines; i++) {
            rrpv[i] = rpvMax;  // initialize to max (distant reuse)
        }
    }

    ~SRRIPReplPolicy() {
        gm_free(rrpv);
    }

    /* Called on cache hit */
    void update(uint32_t id, const MemReq* req) {
        // Hit Priority (HP): set RRPV to 0 on a hit
        rrpv[id] = 0;
    }

    /* Called when a line is replaced */
    void replaced(uint32_t id) {
        // When line is inserted, set RRPV = rpvMax - 1 (long re-reference interval)
        rrpv[id] = rpvMax - 1;
    }

    /* Candidate selection function */ 
    template <typename C>
    inline uint32_t rank(const MemReq* req, C cands) {
        // Find first block with RRPV == rpvMax
        while (true) {
            for (auto ci = cands.begin(); ci != cands.end(); ci.inc()) {
                uint32_t id = *ci;
                if (rrpv[id] == rpvMax) {
                    return id; // Found victim
                }
            }

            // If none found, increment all (aging)
            for (auto ci = cands.begin(); ci != cands.end(); ci.inc()) {
                uint32_t id = *ci;
                if (rrpv[id] < rpvMax) rrpv[id]++;
            }
        }
    }

    DECL_RANK_BINDINGS;
};

#endif // RRIP_REPL_H_
