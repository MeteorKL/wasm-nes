//
// Created by meteor on 2018/11/8.
//

#include "mapper0.h"

void mapper_init(mmc_t *mmc) {
    DEBUG_MSG("mapper_0 init\n");
    mmc->prgRam = malloc(0x2000); // PRG RAM: 2K or 4K in Family Basic only
}

void mapper_free() {
    DEBUG_MSG("mapper_0 free\n");
}

uint8_t mapper_cpu_read(mmc_t *mmc, uint16_t addr) {
    TRACE_MSG("mapper_cpu_read: %x\n", addr);
    if (addr >= 0xc000) {
        TRACE_MSG("prg_rom offset: %u, %x\n", mmc->prg_rom_bank_c000, mmc->prg_rom_bank_c000 * 0x4000 + addr - 0xc000);
        return mmc->cart->prg_rom[mmc->prg_rom_bank_c000 * 0x4000 + addr - 0xc000];
    } else if (addr >= 0x8000) {
        TRACE_MSG("prg_rom offset: %u, %x\n", mmc->prg_rom_bank_8000, mmc->prg_rom_bank_8000 * 0x4000 + addr - 0x8000);
        return mmc->cart->prg_rom[mmc->prg_rom_bank_8000 * 0x4000 + addr - 0x8000];
    } else if (addr >= 0x6000) {
        return mmc->prgRam[addr - 0x6000];
    }
    ASSERT(false);
    return 0;
}

void mapper_cpu_write(mmc_t *mmc, uint16_t addr, uint8_t val) {
    if (addr >= 0xc000) {
        mmc->cart->prg_rom[mmc->prg_rom_bank_c000 * 0x4000 + addr - 0xc000] = val;
    } else if (addr >= 0x8000) {
        mmc->cart->prg_rom[mmc->prg_rom_bank_8000 * 0x4000 + addr - 0x8000] = val;
    } else if (addr >= 0x6000) {
        mmc->prgRam[addr - 0x6000] = val;
    }
}

uint8_t mapper_ppu_read(mmc_t *mmc, uint16_t addr) {
    if (addr < 0x2000) {
        return mmc->cart->chr_rom[addr];
    }
    ASSERT(false);
    return 0;
}

void mapper_ppu_write(mmc_t *mmc, uint16_t addr, uint8_t val) {
    if (addr < 0x2000) {
        mmc->cart->chr_rom[addr] = val;
    }
}