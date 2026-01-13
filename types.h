#ifndef TYPES_H
#define TYPES_H

/**
 * The SBas function:
 * Pointer to a function that takes `n` parameters (up to 3)
 * and returns a signed integer (32 bits)
 */
typedef int (*funcp)();

/**
 * Maps each `line` in a SBas file to its `offset` in the machine code buffer
 */
typedef struct {
  unsigned line;  // line in the text file (1-indexed)
  int offset;     // index of the start of the line's instruction bytes in the machine code buffer
} LineTable;

/**
 * Maps a `sourceOffset` in the buffer requesting a jump
 * to a desired `targetLine` or `targetOffset`
 */
typedef struct {
  unsigned char isLine;  // whether `target` is a line in SBas source file or an offset in the byte buffer
  union {
    unsigned targetLine;
    int targetOffset;
  } target;
  int sourceOffset;  // first byte of the placeholder bytes to be patched
} RelocationTable;

/**
 * An abstraction of a x86-64 machine code instruction
 */
typedef struct {
  /** The base operation code
   * If > 0xFF, the emitter automatically handles the endianness
   * (emitting high byte then low byte).
   */
  unsigned int opcode;

  /**
   * If true, emits the REX.W prefix.
   * Promotes 32-bit operations to 64-bit.
   */
  unsigned char is_64bit;

  /**
   * Enables emission of the ModRM byte
   */
  unsigned char use_modrm;

  /**
   * Bits 7-6 of ModRM. Sets up the addressing mode.
   */
  unsigned char mod;

  /**
   * Bits 5-3 of ModRM. This field is sometimes overloaded in x86-64.
   * It can be used for:
   * 1. destination register
   * 2. opcode extension
   */
  unsigned char reg;

  /**
   * Bits 2-0 of ModRM.
   * Specifies source register of the operation
   */
  unsigned char rm;

  /**
   * Enables emission of immediate bytes at the end of an instruction.
   */
  unsigned char use_imm;

  /**
   *  The constant value (e.g., -1024, 0, 256, etc)
   */
  int immediate;

  /**
   * Size of the immediate in bytes (1 or 4)
   */
  unsigned char imm_size;

  /** Flag for "Short Move" (0xB8 + rd).
   * This optimization embeds the destination register ID into the lower
   * 3 bits of the opcode itself, skipping the ModR/M byte entirely.
   */
  unsigned char is_imm_mov;

  /**
   * The register ID to be added to the base opcode (for `is_imm_mov`).
   */
  int imm_mov_rd;

  /**
   * Treats the instruction to be generated as a comparison between values.
   * Used for conditional jumps.
   */
  unsigned char isCmp;
} Instruction;

#endif