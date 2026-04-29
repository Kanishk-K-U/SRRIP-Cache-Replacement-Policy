# SRRIP Cache Replacement Policy

## Overview

This project implements and evaluates **Static Re-Reference Interval Prediction – Hit Priority (SRRIP-HP)** as a cache replacement policy for the shared **Last-Level Cache (LLC/L3)** in the `zsim` architectural simulator.

The goal of this project is to compare SRRIP against traditional cache replacement policies such as **Least Recently Used (LRU)** and **Least Frequently Used (LFU)** using single-core and multicore benchmark workloads.

SRRIP improves cache efficiency by predicting how soon a cache block is likely to be reused and reducing cache pollution from scan-like memory access patterns.

---

## Project Objectives

- Implement **SRRIP-HP** cache replacement policy in `zsim`
- Integrate SRRIP as a selectable LLC replacement policy
- Compare **LRU, LFU, and SRRIP** across benchmark workloads
- Evaluate performance using:
  - Total execution cycles
  - Instructions Per Cycle (IPC)
  - L3 Misses Per Kilo Instructions (L3 MPKI)

---

## Background

Modern processors rely heavily on efficient cache replacement policies to improve memory hierarchy performance. A poor replacement policy can evict useful cache blocks too early or retain blocks that are unlikely to be reused, leading to higher miss rates and performance loss.

### LRU

**Least Recently Used** evicts the cache block that has not been accessed for the longest time. It works well for workloads with strong temporal locality but struggles with scan-heavy workloads.

### LFU

**Least Frequently Used** evicts the block with the lowest access count. It can retain frequently used data but adapts slowly to changing access behavior.

### SRRIP

**Static Re-Reference Interval Prediction** predicts the future reuse distance of each cache block using a small per-line counter called the **Re-Reference Prediction Value (RRPV)**.

SRRIP is effective because it avoids inserting new blocks as immediately important, reducing pollution caused by data that may not be reused.

---

## SRRIP-HP Algorithm

This project implements the **Hit Priority** version of SRRIP using a 2-bit RRPV counter per cache line.

### RRPV Meaning

| RRPV Value | Meaning |
|---|---|
| `0` | Near-immediate reuse expected |
| `1` | Short reuse interval |
| `2` | Long reuse interval |
| `3` | Distant reuse, eviction candidate |

### Policy Rules

| Event | Action |
|---|---|
| New block insertion | Set `RRPV = 2` |
| Cache hit | Set `RRPV = 0` |
| Victim selection | Choose a block with `RRPV = 3` |
| No victim found | Increment all RRPVs and retry |

---

## Implementation Details

The SRRIP policy was implemented inside the `zsim` source tree.

### Modified Files

```text
zsim/src/rrip_repl.h
zsim/src/init.cpp
