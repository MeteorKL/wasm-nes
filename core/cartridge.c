//
// Created by meteor on 2018/11/4.
//

#include "cartridge.h"

cartridge_t *cartridge_init() {
    cartridge_t *cart = malloc(sizeof(cartridge_t));
    cart->trainer = NULL;
    cart->num_prg_rom_bank = 0;
    cart->prg_rom = NULL;
    cart->num_chr_rom_bank = 0;
    cart->chr_rom = NULL;
    cart->mapper_no = 0;
    return cart;
}

int32_t cartridge_load(cartridge_t *cart, uint8_t *rom, uint32_t rom_size) {
    ASSERT(cart != NULL);
    ASSERT(cart->trainer == NULL);
    ASSERT(cart->prg_rom == NULL);
    ASSERT(cart->chr_rom == NULL);
    if (rom == NULL || rom_size == 0) {
        return EINVALID_ARGUMENT;
    }
    cart->rom_size = rom_size;
    cart->rom = malloc(rom_size);
    memcpy(cart->rom, rom, rom_size);

    uint8_t *rom_end = cart->rom + cart->rom_size;

    uint8_t *rom_ptr = cart->rom;
    ines_header_t *header = (ines_header_t *)rom_ptr;
    printf("%X %X\n", (uint32_t)cart->rom, (uint32_t)rom_end);

    // Identify the rom as an iNES file: NES\x1a.
    if (header->magic != INES_FILE_MAGIC) {
        return EINVALID_INES_HEADER;
    }
    // Reserved for future usage and should all be 0.
    for (uint8_t i = 0; i < sizeof(header->padding); ++i) {
        if (header->padding[i] != 0) {
            return EINVALID_INES_HEADER;
        }
    }

    // ROM Control Byte 1:
    // • Bit 0 - Indicates the type of mirroring used by the game
    //     where 0 indicates horizontal mirroring, 1 indicates
    //     vertical mirroring.
    // • Bit 1 - Indicates the presence of battery-backed RAM at
    //     memory locations $6000-$7FFF.
    // • Bit 2 - Indicates the presence of a 512-byte trainer at
    //     memory locations $7000-$71FF.
    // • Bit 3 - If this bit is set it overrides bit 0 to indicate fourscreen
    //     mirroring should be used.
    // • Bits 4-7 - Four lower bits of the mapper number.

    // ROM Control Byte 2:
    // • Bits 0-3 - Reserved for future usage and should all be 0.
    // • Bits 4-7 - Four upper bits of the mapper number.
    cart->mapper_no = (header->control_2 & 0xf0) | (header->control_1 >> 4);
    DEBUG_MSG("using mapper: %u\n", cart->mapper_no);

    cart->mirror = ((header->control_1 >> 2) & 0b10) | (header->control_1 & 1);

    // battery-backed RAM
    bool battery = (header->control_1 >> 1) & 1;
    DEBUG_MSG("battery: %d\n", battery);
    DEBUG_MSG("trainer: %d\n", header->control_1 & 0b100);
    rom_ptr += sizeof(ines_header_t);

    // Following the header is the 512-byte trainer, if one is present,
    // otherwise the ROM banks begin here, starting with PRG-ROM then CHR-ROM.
    if ((header->control_1 & 0b100) == 0b100) {
        cart->trainer = rom_ptr;
        rom_ptr += TRAINER_SIZE;
    }

    // Load PRG-ROM banks:
    cart->prg_rom = rom_ptr;
    rom_ptr += header->num_prg_rom_bank * PRG_ROM_SIZE;
    if (header->num_chr_rom_bank != 0) {
        cart->is_chr_ram = false;
        // Load CHR-ROM banks:
        cart->chr_rom = rom_ptr;
        rom_ptr += header->num_chr_rom_bank * CHR_ROM_SIZE;
    } else {
        // Provide CHR-ROM and CHR-RAM if not in the rom:
        header->num_chr_rom_bank = 1;
        cart->is_chr_ram = true;
        cart->chr_rom = malloc(CHR_ROM_SIZE);
    }
    cart->num_prg_rom_bank = header->num_prg_rom_bank;
    cart->num_chr_rom_bank = header->num_chr_rom_bank;

    DEBUG_MSG("num_prg_rom_bank:%u, num_chr_rom_bank:%u is_chr_ram: %u\n", cart->num_prg_rom_bank,
              cart->num_chr_rom_bank, cart->is_chr_ram);

    // Check if the contents of rom have been completely read.
    if (rom_ptr != rom_end) {
        DEBUG_MSG("%X %X\n", (uint32_t)rom_ptr, (uint32_t)rom_end);
        return EINVALID_INES_CONTENT;
    }

    return EOK;
}

void cartridge_free(cartridge_t **cart) {
    ASSERT((*cart) != NULL);
    if ((*cart)->is_chr_ram) {
        free((*cart)->chr_rom);
    }
    (*cart)->trainer = NULL;
    (*cart)->num_prg_rom_bank = 0;
    (*cart)->prg_rom = NULL;
    (*cart)->num_chr_rom_bank = 0;
    (*cart)->chr_rom = NULL;
    (*cart)->mapper_no = 0;
}