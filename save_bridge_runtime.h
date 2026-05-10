#ifndef SAVE_BRIDGE_RUNTIME_H
#define SAVE_BRIDGE_RUNTIME_H

#include <gb/gb.h>
#include "save_runtime.h"

void save_bridge_build_snapshot(SaveSnapshot *out) BANKED;
void save_bridge_apply_snapshot(const SaveSnapshot *in) BANKED;

#endif
