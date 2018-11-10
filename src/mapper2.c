//
// Created by meteor on 2018/11/8.
//

#include "mapper2.h"

void mapper_init(mmc_t *mmc)
{
    DEBUG_MSG("mapper_2 init\n");
    TRACE_MSG("mmc->cart->chr_ram: %X\n", (uint32_t)(mmc->cart->chr_ram));
    ASSERT(mmc->cart->chr_ram != NULL);
}

void mapper_free()
{
    DEBUG_MSG("mapper_2 free\n");
}

uint8_t cpu_read(mmc_t *mmc, uint16_t addr)
{
    if (addr >= 0xc000)
    {
        return mmc->cart->prg_rom[mmc->prg_rom_bank_c000 * 0x4000 + addr - 0xc000];
    }
    else if (addr >= 0x8000)
    {
        return mmc->cart->prg_rom[mmc->prg_rom_bank_8000 * 0x4000 + addr - 0x8000];
    }
    ASSERT(false);
}

void cpu_write(mmc_t *mmc, uint16_t addr, uint8_t value)
{
    if (addr >= 0xc000)
    {
        mmc->cart->prg_rom[mmc->prg_rom_bank_c000 * 0x4000 + addr - 0xc000] = value;
    }
    else if (addr >= 0x8000)
    {
        mmc->cart->prg_rom[mmc->prg_rom_bank_8000 * 0x4000 + addr - 0x8000] = value;
    }
}

uint8_t ppu_read(mmc_t *mmc, uint16_t addr)
{
    if (addr < 0x2000)
    {
        return mmc->cart->chr_rom[addr];
    }
    else if (addr >= 0x6000)
    {
        return mmc->cart->chr_ram[addr - 0x6000];
    }
    ASSERT(false);
}

void ppu_write(mmc_t *mmc, uint16_t addr, uint8_t value)
{
    if (addr < 0x2000)
    {
        mmc->cart->chr_rom[addr] = value;
    }
    else if (addr >= 0x6000)
    {
        mmc->cart->chr_ram[addr - 0x6000] = value;
    }
}