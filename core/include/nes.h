//
// Created by meteor on 2018/11/5.
//

#ifndef WASM_NES_NES_H
#define WASM_NES_NES_H

#include "apu.h"
#include "cartridge.h"
#include "controller.h"
#include "cpu.h"
#include "mmc.h"
#include "port.h"
#include "ppu.h"

typedef struct _nes_t {
    cartridge_t *cart;
    mmc_t *mmc;
    cpu_t *cpu;
    ppu_t *ppu;
    apu_t *apu;
    controller_t controller[2];
} nes_t;

nes_t *nes_init();
void nes_reset(nes_t **nes);
int32_t nes_load(nes_t *nes, uint8_t *data, uint32_t data_len);
int32_t nes_load_file(nes_t *nes, const char *filepath);
void step_frame(nes_t *nes);
#endif // WASM_NES_NES_H
