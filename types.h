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
 * Maps a desired `target` line OR offset to jump to to a requesting `offset` in the buffer
 */
typedef struct {
  unsigned char isLine;  // whether the jump target is a line in SBas source file or an offset in the byte buffer
  int target;            // line or offset to jump to
  int offset;            // first byte of the placeholder bytes to be patched
} RelocationTable;

/**
 * @brief An abstraction of a x86-64 machine code instruction:
 *
 * it serves like a "filling form" for the callers of the `emit_instruction` function,
 * abstracting away the required bit-packing for the REX Prefix, ModRM,
 * SIB, and displacement bytes.
 */
typedef struct {
  /** The base operation code
   * If > 0xFF, the emitter automatically handles the endianness
   * (emitting high byte then low byte).
   */
  unsigned int opcode;

  /**
   * If true, emits the REX.W prefix (0x48).
   * Used to promote 32-bit operations to 64-bit.
   */
  unsigned char is_64bit;

  /**
   * Enables emission of the ModRM byte
   */
  unsigned char use_modrm;

  /**
   * Bits 7-6 of ModRM. Sets up the addressing mode.
   * In SBas two modes are used:
   * - 3 (11): Register-Direct
   * - 1 (01): Memory + 8-bit Displacement
   */
  unsigned char mod;

  /**
   * Bits 5-3 of ModRM. This field is overloaded in x86:
   * 1. Used for destination register
   * 2. Used for opcode extension
   */
  unsigned char reg;

  /**
   * Bits 2-0 of ModRM.
   * Specifies either the source register
   * or the base register for memory access
   */
  unsigned char rm;

  /**
   * Enables emission of a displacement byte after the ModR/M byte.
   */
  unsigned char use_disp;

  /**
   * The small offset added to the base address stored in a register, for instance -8(%rbp)
   */
  signed char displacement;

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
   * Treats the instruction to be generated as an arithmetic operation
   */
  unsigned char isArithmOp;

  /**
   * Treats the instruction to be generated as a comparison between values.
   * Used for conditional jumps.
   */
  unsigned char isCmp;
} Instruction;

#endif