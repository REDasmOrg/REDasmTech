#pragma once

#include <redasm/redasm.h>

// clang-format off
template<typename T> T elf_st_bind(T i) { return i >> 4; }
template<typename T> T elf_st_type(T i) { return i & 0xF; }
template<typename T> T elf_st_info(T b, T t) { return (b << 4) | (t & 0xF); }
template<typename T> T elf_st_visibility(T o) { return o >> 0x3; }
// clang-format on

constexpr u8 ELFMAG0 = 0x7f;
constexpr char ELFMAG1 = 'E';
constexpr char ELFMAG2 = 'L';
constexpr char ELFMAG3 = 'F';

constexpr int ELFCLASSNONE = 0;
constexpr int ELFCLASS32 = 1;
constexpr int ELFCLASS64 = 2;

constexpr int ELFDATANONE = 0;
constexpr int ELFDATA2LSB = 1;
constexpr int ELFDATA2MSB = 2;

constexpr int EV_NONE = 0;
constexpr int EV_CURRENT = 1;

constexpr int EM_NONE = 0;         /* No machine */
constexpr int EM_M32 = 1;          /* AT&T WE 32100 */
constexpr int EM_SPARC = 2;        /* SUN SPARC */
constexpr int EM_386 = 3;          /* Intel 80386 */
constexpr int EM_68K = 4;          /* Motorola m68k family */
constexpr int EM_88K = 5;          /* Motorola m88k family */
constexpr int EM_IAMCU = 6;        /* Intel MCU */
constexpr int EM_860 = 7;          /* Intel 80860 */
constexpr int EM_MIPS = 8;         /* MIPS R3000 big-endian */
constexpr int EM_S370 = 9;         /* IBM System/370 */
constexpr int EM_MIPS_RS3_LE = 10; /* MIPS R3000 little-endian */
/* reserved 11-14 */
constexpr int EM_PARISC = 15; /* HPPA */
/* reserved 16 */
constexpr int EM_VPP500 = 17;      /* Fujitsu VPP500 */
constexpr int EM_SPARC32PLUS = 18; /* Sun's "v8plus" */
constexpr int EM_960 = 19;         /* Intel 80960 */
constexpr int EM_PPC = 20;         /* PowerPC */
constexpr int EM_PPC64 = 21;       /* PowerPC 64-bit */
constexpr int EM_S390 = 22;        /* IBM S390 */
constexpr int EM_SPU = 23;         /* IBM SPU/SPC */
/* reserved 24-35 */
constexpr int EM_V800 = 36;       /* NEC V800 series */
constexpr int EM_FR20 = 37;       /* Fujitsu FR20 */
constexpr int EM_RH32 = 38;       /* TRW RH-32 */
constexpr int EM_RCE = 39;        /* Motorola RCE */
constexpr int EM_ARM = 40;        /* ARM */
constexpr int EM_FAKE_ALPHA = 41; /* Digital Alpha */
constexpr int EM_SH = 42;         /* Hitachi SH */
constexpr int EM_SPARCV9 = 43;    /* SPARC v9 64-bit */
constexpr int EM_TRICORE = 44;    /* Siemens Tricore */
constexpr int EM_ARC = 45;        /* Argonaut RISC Core */
constexpr int EM_H8_300 = 46;     /* Hitachi H8/300 */
constexpr int EM_H8_300H = 47;    /* Hitachi H8/300H */
constexpr int EM_H8S = 48;        /* Hitachi H8S */
constexpr int EM_H8_500 = 49;     /* Hitachi H8/500 */
constexpr int EM_IA_64 = 50;      /* Intel Merced */
constexpr int EM_MIPS_X = 51;     /* Stanford MIPS-X */
constexpr int EM_COLDFIRE = 52;   /* Motorola Coldfire */
constexpr int EM_68HC12 = 53;     /* Motorola M68HC12 */
constexpr int EM_MMA = 54;        /* Fujitsu MMA Multimedia Accelerator */
constexpr int EM_PCP = 55;        /* Siemens PCP */
constexpr int EM_NCPU = 56;       /* Sony nCPU embeeded RISC */
constexpr int EM_NDR1 = 57;       /* Denso NDR1 microprocessor */
constexpr int EM_STARCORE = 58;   /* Motorola Start*Core processor */
constexpr int EM_ME16 = 59;       /* Toyota ME16 processor */
constexpr int EM_ST100 = 60;      /* STMicroelectronic ST100 processor */
constexpr int EM_TINYJ = 61;      /* Advanced Logic Corp. Tinyj emb.fam */
constexpr int EM_X86_64 = 62;     /* AMD x86-64 architecture */
constexpr int EM_PDSP = 63;       /* Sony DSP Processor */
constexpr int EM_PDP10 = 64;      /* Digital PDP-10 */
constexpr int EM_PDP11 = 65;      /* Digital PDP-11 */
constexpr int EM_FX66 = 66;       /* Siemens FX66 microcontroller */
constexpr int EM_ST9PLUS = 67;    /* STMicroelectronics ST9+ 8/16 mc */
constexpr int EM_ST7 = 68;        /* STmicroelectronics ST7 8 bit mc */
constexpr int EM_68HC16 = 69;     /* Motorola MC68HC16 microcontroller */
constexpr int EM_68HC11 = 70;     /* Motorola MC68HC11 microcontroller */
constexpr int EM_68HC08 = 71;     /* Motorola MC68HC08 microcontroller */
constexpr int EM_68HC05 = 72;     /* Motorola MC68HC05 microcontroller */
constexpr int EM_SVX = 73;        /* Silicon Graphics SVx */
constexpr int EM_ST19 = 74;       /* STMicroelectronics ST19 8 bit mc */
constexpr int EM_VAX = 75;        /* Digital VAX */
constexpr int EM_CRIS = 76;       /* Axis Communications 32-bit emb.proc */
constexpr int EM_JAVELIN = 77;    /* Infineon Technologies 32-bit emb.proc */
constexpr int EM_FIREPATH = 78;   /* Element 14 64-bit DSP Processor */
constexpr int EM_ZSP = 79;        /* LSI Logic 16-bit DSP Processor */
constexpr int EM_MMIX = 80;       /* Donald Knuth's educational 64-bit proc */
constexpr int EM_HUANY = 81;      /* Harvard machine-independent object files */
constexpr int EM_PRISM = 82;      /* SiTera Prism */
constexpr int EM_AVR = 83;        /* Atmel AVR 8-bit microcontroller */
constexpr int EM_FR30 = 84;       /* Fujitsu FR30 */
constexpr int EM_D10V = 85;       /* Mitsubishi D10V */
constexpr int EM_D30V = 86;       /* Mitsubishi D30V */
constexpr int EM_V850 = 87;       /* NEC v850 */
constexpr int EM_M32R = 88;       /* Mitsubishi M32R */
constexpr int EM_MN10300 = 89;    /* Matsushita MN10300 */
constexpr int EM_MN10200 = 90;    /* Matsushita MN10200 */
constexpr int EM_PJ = 91;         /* picoJava */
constexpr int EM_OPENRISC = 92;   /* OpenRISC 32-bit embedded processor */
constexpr int EM_ARC_COMPACT = 93; /* ARC International ARCompact */
constexpr int EM_XTENSA = 94;      /* Tensilica Xtensa Architecture */
constexpr int EM_VIDEOCORE = 95;   /* Alphamosaic VideoCore */
constexpr int EM_TMM_GPP = 96;   /* Thompson Multimedia General Purpose Proc */
constexpr int EM_NS32K = 97;     /* National Semi. 32000 */
constexpr int EM_TPC = 98;       /* Tenor Network TPC */
constexpr int EM_SNP1K = 99;     /* Trebia SNP 1000 */
constexpr int EM_ST200 = 100;    /* STMicroelectronics ST200 */
constexpr int EM_IP2K = 101;     /* Ubicom IP2xxx */
constexpr int EM_MAX = 102;      /* MAX processor */
constexpr int EM_CR = 103;       /* National Semi. CompactRISC */
constexpr int EM_F2MC16 = 104;   /* Fujitsu F2MC16 */
constexpr int EM_MSP430 = 105;   /* Texas Instruments msp430 */
constexpr int EM_BLACKFIN = 106; /* Analog Devices Blackfin DSP */
constexpr int EM_SE_C33 = 107;   /* Seiko Epson S1C33 family */
constexpr int EM_SEP = 108;      /* Sharp embedded microprocessor */
constexpr int EM_ARCA = 109;     /* Arca RISC */
constexpr int EM_UNICORE = 110;  /* PKU-Unity & MPRC Peking Uni. mc series */
constexpr int EM_EXCESS = 111;   /* eXcess configurable cpu */
constexpr int EM_DXP = 112;      /* Icera Semi. Deep Execution Processor */
constexpr int EM_ALTERA_NIOS2 = 113; /* Altera Nios II */
constexpr int EM_CRX = 114;          /* National Semi. CompactRISC CRX */
constexpr int EM_XGATE = 115;        /* Motorola XGATE */
constexpr int EM_C166 = 116;         /* Infineon C16x/XC16x */
constexpr int EM_M16C = 117;         /* Renesas M16C */
constexpr int EM_DSPIC30F = 118;     /* Microchip Technology dsPIC30F */
constexpr int EM_CE = 119;           /* Freescale Communication Engine RISC */
constexpr int EM_M32C = 120;         /* Renesas M32C */
/* reserved 121-130 */
constexpr int EM_TSK3000 = 131;       /* Altium TSK3000 */
constexpr int EM_RS08 = 132;          /* Freescale RS08 */
constexpr int EM_SHARC = 133;         /* Analog Devices SHARC family */
constexpr int EM_ECOG2 = 134;         /* Cyan Technology eCOG2 */
constexpr int EM_SCORE7 = 135;        /* Sunplus S+core7 RISC */
constexpr int EM_DSP24 = 136;         /* New Japan Radio (NJR) 24-bit DSP */
constexpr int EM_VIDEOCORE3 = 137;    /* Broadcom VideoCore III */
constexpr int EM_LATTICEMICO32 = 138; /* RISC for Lattice FPGA */
constexpr int EM_SE_C17 = 139;        /* Seiko Epson C17 */
constexpr int EM_TI_C6000 = 140;      /* Texas Instruments TMS320C6000 DSP */
constexpr int EM_TI_C2000 = 141;      /* Texas Instruments TMS320C2000 DSP */
constexpr int EM_TI_C5500 = 142;      /* Texas Instruments TMS320C55x DSP */
constexpr int EM_TI_ARP32 = 143;      /* Texas Instruments App. Specific RISC */
constexpr int EM_TI_PRU = 144; /* Texas Instruments Prog. Realtime Unit */
/* reserved 145-159 */
constexpr int EM_MMDSP_PLUS = 160;  /* STMicroelectronics 64bit VLIW DSP */
constexpr int EM_CYPRESS_M8C = 161; /* Cypress M8C */
constexpr int EM_R32C = 162;        /* Renesas R32C */
constexpr int EM_TRIMEDIA = 163;    /* NXP Semi. TriMedia */
constexpr int EM_QDSP6 = 164;       /* QUALCOMM DSP6 */
constexpr int EM_8051 = 165;        /* Intel 8051 and variants */
constexpr int EM_STXP7X = 166;      /* STMicroelectronics STxP7x */
constexpr int EM_NDS32 = 167;       /* Andes Tech. compact code emb. RISC */
constexpr int EM_ECOG1X = 168;      /* Cyan Technology eCOG1X */
constexpr int EM_MAXQ30 = 169;      /* Dallas Semi. MAXQ30 mc */
constexpr int EM_XIMO16 = 170;      /* New Japan Radio (NJR) 16-bit DSP */
constexpr int EM_MANIK = 171;       /* M2000 Reconfigurable RISC */
constexpr int EM_CRAYNV2 = 172;     /* Cray NV2 vector architecture */
constexpr int EM_RX = 173;          /* Renesas RX */
constexpr int EM_METAG = 174;       /* Imagination Tech. META */
constexpr int EM_MCST_ELBRUS = 175; /* MCST Elbrus */
constexpr int EM_ECOG16 = 176;      /* Cyan Technology eCOG16 */
constexpr int EM_CR16 = 177;        /* National Semi. CompactRISC CR16 */
constexpr int EM_ETPU = 178;  /* Freescale Extended Time Processing Unit */
constexpr int EM_SLE9X = 179; /* Infineon Tech. SLE9X */
constexpr int EM_L10M = 180;  /* Intel L10M */
constexpr int EM_K10M = 181;  /* Intel K10M */
/* reserved 182 */
constexpr int EM_AARCH64 = 183; /* ARM AARCH64 */
/* reserved 184 */
constexpr int EM_AVR32 = 185;        /* Amtel 32-bit microprocessor */
constexpr int EM_STM8 = 186;         /* STMicroelectronics STM8 */
constexpr int EM_TILE64 = 187;       /* Tileta TILE64 */
constexpr int EM_TILEPRO = 188;      /* Tilera TILEPro */
constexpr int EM_MICROBLAZE = 189;   /* Xilinx MicroBlaze */
constexpr int EM_CUDA = 190;         /* NVIDIA CUDA */
constexpr int EM_TILEGX = 191;       /* Tilera TILE-Gx */
constexpr int EM_CLOUDSHIELD = 192;  /* CloudShield */
constexpr int EM_COREA_1ST = 193;    /* KIPO-KAIST Core-A 1st gen. */
constexpr int EM_COREA_2ND = 194;    /* KIPO-KAIST Core-A 2nd gen. */
constexpr int EM_ARC_COMPACT2 = 195; /* Synopsys ARCompact V2 */
constexpr int EM_OPEN8 = 196;        /* Open8 RISC */
constexpr int EM_RL78 = 197;         /* Renesas RL78 */
constexpr int EM_VIDEOCORE5 = 198;   /* Broadcom VideoCore V */
constexpr int EM_78KOR = 199;        /* Renesas 78KOR */
constexpr int EM_56800EX = 200;      /* Freescale 56800EX DSC */
constexpr int EM_BA1 = 201;          /* Beyond BA1 */
constexpr int EM_BA2 = 202;          /* Beyond BA2 */
constexpr int EM_XCORE = 203;        /* XMOS xCORE */
constexpr int EM_MCHP_PIC = 204;     /* Microchip 8-bit PIC(r) */
/* reserved 205-209 */
constexpr int EM_KM32 = 210;        /* KM211 KM32 */
constexpr int EM_KMX32 = 211;       /* KM211 KMX32 */
constexpr int EM_EMX16 = 212;       /* KM211 KMX16 */
constexpr int EM_EMX8 = 213;        /* KM211 KMX8 */
constexpr int EM_KVARC = 214;       /* KM211 KVARC */
constexpr int EM_CDP = 215;         /* Paneve CDP */
constexpr int EM_COGE = 216;        /* Cognitive Smart Memory Processor */
constexpr int EM_COOL = 217;        /* Bluechip CoolEngine */
constexpr int EM_NORC = 218;        /* Nanoradio Optimized RISC */
constexpr int EM_CSR_KALIMBA = 219; /* CSR Kalimba */
constexpr int EM_Z80 = 220;         /* Zilog Z80 */
constexpr int EM_VISIUM = 221;      /* Controls and Data Services VISIUMcore */
constexpr int EM_FT32 = 222;        /* FTDI Chip FT32 */
constexpr int EM_MOXIE = 223;       /* Moxie processor */
constexpr int EM_AMDGPU = 224;      /* AMD GPU */
/* reserved 225-242 */
constexpr int EM_RISCV = 243; /* RISC-V */

constexpr int DT_NULL = 0;             /* Marks end of dynamic section */
constexpr int DT_NEEDED = 1;           /* Name of needed library */
constexpr int DT_PLTRELSZ = 2;         /* Size in bytes of PLT relocs */
constexpr int DT_PLTGOT = 3;           /* Processor defined value */
constexpr int DT_HASH = 4;             /* Address of symbol hash table */
constexpr int DT_STRTAB = 5;           /* Address of string table */
constexpr int DT_SYMTAB = 6;           /* Address of symbol table */
constexpr int DT_RELA = 7;             /* Address of Rela relocs */
constexpr int DT_RELASZ = 8;           /* Total size of Rela relocs */
constexpr int DT_RELAENT = 9;          /* Size of one Rela reloc */
constexpr int DT_STRSZ = 10;           /* Size of string table */
constexpr int DT_SYMENT = 11;          /* Size of one symbol table entry */
constexpr int DT_INIT = 12;            /* Address of init function */
constexpr int DT_FINI = 13;            /* Address of termination function */
constexpr int DT_SONAME = 14;          /* Name of shared object */
constexpr int DT_RPATH = 15;           /* Library search path (deprecated) */
constexpr int DT_SYMBOLIC = 16;        /* Start symbol search here */
constexpr int DT_REL = 17;             /* Address of Rel relocs */
constexpr int DT_RELSZ = 18;           /* Total size of Rel relocs */
constexpr int DT_RELENT = 19;          /* Size of one Rel reloc */
constexpr int DT_PLTREL = 20;          /* Type of reloc in PLT */
constexpr int DT_DEBUG = 21;           /* For debugging; unspecified */
constexpr int DT_TEXTREL = 22;         /* Reloc might modify .text */
constexpr int DT_JMPREL = 23;          /* Address of PLT relocs */
constexpr int DT_BIND_NOW = 24;        /* Process relocations of object */
constexpr int DT_INIT_ARRAY = 25;      /* Array with addresses of init fct */
constexpr int DT_FINI_ARRAY = 26;      /* Array with addresses of fini fct */
constexpr int DT_INIT_ARRAYSZ = 27;    /* Size in bytes of DT_INIT_ARRAY */
constexpr int DT_FINI_ARRAYSZ = 28;    /* Size in bytes of DT_FINI_ARRAY */
constexpr int DT_RUNPATH = 29;         /* Library search path */
constexpr int DT_FLAGS = 30;           /* Flags for the object being loaded */
constexpr int DT_ENCODING = 32;        /* Start of encoded range */
constexpr int DT_PREINIT_ARRAY = 32;   /* Array with addresses of preinit fct*/
constexpr int DT_PREINIT_ARRAYSZ = 33; /* size in bytes of DT_PREINIT_ARRAY */
constexpr int DT_LOOS = 0x6000000d;
constexpr int DT_SUNW_RTLDINF = 0x6000000e;
constexpr int DT_HIOS = 0x6ffff000;
constexpr int DT_VALRNGLO = 0x6ffffd00;
constexpr int DT_CHECKSUM = 0x6ffffdf8;
constexpr int DT_PLTPADSZ = 0x6ffffdf9;
constexpr int DT_MOVEENT = 0x6ffffdfa;
constexpr int DT_MOVESZ = 0x6ffffdfb;
constexpr int DT_FEATURE_1 = 0x6ffffdfc;
constexpr int DT_POSFLAG_1 = 0x6ffffdfd;
constexpr int DT_SYMINSZ = 0x6ffffdfe;
constexpr int DT_SYMINENT = 0x6ffffdff;
constexpr int DT_VALRNGHI = 0x6ffffdff;
constexpr int DT_ADDRRNGLO = 0x6ffffe00;
constexpr int DT_CONFIG = 0x6ffffefa;
constexpr int DT_DEPAUDIT = 0x6ffffefb;
constexpr int DT_AUDIT = 0x6ffffefc;
constexpr int DT_PLTPAD = 0x6ffffefd;
constexpr int DT_MOVETAB = 0x6ffffefe;
constexpr int DT_SYMINFO = 0x6ffffeff;
constexpr int DT_ADDRRNGHI = 0x6ffffeff;
constexpr int DT_VERSYM = 0x6ffffff0;
constexpr int DT_RELACOUNT = 0x6ffffff9;
constexpr int DT_RELCOUNT = 0x6ffffffa;
constexpr int DT_FLAGS_1 = 0x6ffffffb;
constexpr int DT_VERDEF = 0x6ffffffc;
constexpr int DT_VERDEFNUM = 0x6ffffffd;
constexpr int DT_VERNEED = 0x6ffffffe;
constexpr int DT_VERNEEDNUM = 0x6fffffff;
constexpr int DT_LOPROC = 0x70000000;
constexpr int DT_SPARC_REGISTER = 0x70000001;
constexpr int DT_AUXILIARY = 0x7ffffffd;
constexpr int DT_USED = 0x7ffffffe;
constexpr int DT_FILTER = 0x7fffffff;
constexpr int DT_HIPROC = 0x7fffffff;

constexpr int PT_NULL = 0;
constexpr int PT_LOAD = 1;
constexpr int PT_DYNAMIC = 2;
constexpr int PT_INTERP = 3;
constexpr int PT_NOTE = 4;
constexpr int PT_SHLIB = 5;
constexpr int PT_PHDR = 6;
constexpr int PT_LOOS = 0x60000000;
constexpr int PT_HIOS = 0x6fffffff;
constexpr int PT_LOPROC = 0x70000000;
constexpr int PT_HIPROC = 0x7fffffff;
constexpr int PT_GNU_EH_FRAME = 0x6474e550;

constexpr int PF_X = 0x1;
constexpr int PF_W = 0x2;
constexpr int PF_R = 0x4;

constexpr int SHN_UNDEF = 0;
constexpr int SHN_LORESERVE = 0xff00;
constexpr int SHN_LOPROC = 0xff00;
constexpr int SHN_HIPROC = 0xff1f;
constexpr int SHN_ABS = 0xfff1;
constexpr int SHN_COMMON = 0xfff2;
constexpr int SHN_HIRESERVE = 0xffff;

constexpr int SHT_NULL = 0;
constexpr int SHT_PROGBITS = 1;
constexpr int SHT_SYMTAB = 2;
constexpr int SHT_STRTAB = 3;
constexpr int SHT_RELA = 4;
constexpr int SHT_HASH = 5;
constexpr int SHT_DYNAMIC = 6;
constexpr int SHT_NOTE = 7;
constexpr int SHT_NOBITS = 8;
constexpr int SHT_REL = 9;
constexpr int SHT_SHLIB = 10;
constexpr int SHT_DYNSYM = 11;
constexpr int SHT_NUM = 12;
constexpr int SHT_INIT_ARRAY = 14;
constexpr int SHT_FINI_ARRAY = 15;
constexpr int SHT_PREINIT_ARRAY = 16;
constexpr int SHT_GROUP = 17;
constexpr int SHT_GNU_VERDEF = 0x6ffffffd;
constexpr int SHT_GNU_VERNEED = 0x6ffffffe;
constexpr int SHT_GNU_VERSYM = 0x6fffffff;
constexpr int SHT_SYMTAB_SHNDX = 0x60000000;
constexpr int SHT_LOPROC = 0x70000000;
constexpr int SHT_HIPROC = 0x7fffffff;
constexpr int SHT_LOUSER = 0x80000000;
constexpr int SHT_HIUSER = 0xffffffff;

constexpr int SHF_WRITE = 0x1;
constexpr int SHF_ALLOC = 0x2;
constexpr int SHF_EXECINSTR = 0x4;
constexpr int SHF_MASKPROC = 0xf0000000;

constexpr int STB_LOCAL = 0;
constexpr int STB_GLOBAL = 1;
constexpr int STB_WEAK = 2;

constexpr int STT_NOTYPE = 0;
constexpr int STT_OBJECT = 1;
constexpr int STT_FUNC = 2;
constexpr int STT_SECTION = 3;
constexpr int STT_FILE = 4;
constexpr int STT_COMMON = 5;
constexpr int STT_TLS = 6;
constexpr int STT_LOPROC = 13;
constexpr int STT_HIPROC = 15;

constexpr int STT_ARM_TFUNC = STT_LOPROC;

constexpr int STV_DEFAULT = 0;
constexpr int STV_INTERNAL = 1;
constexpr int STV_HIDDEN = 2;

constexpr int STV_PROTECTED = 3;

constexpr int EF_MIPS_ARCH_1 = 0x00000000;
constexpr int EF_MIPS_ARCH_2 = 0x10000000;
constexpr int EF_MIPS_ARCH_3 = 0x20000000;
constexpr int EF_MIPS_ARCH_4 = 0x30000000;
constexpr int EF_MIPS_ARCH_5 = 0x40000000;
constexpr int EF_MIPS_ARCH_32 = 0x50000000;
constexpr int EF_MIPS_ARCH_64 = 0x60000000;
constexpr int EF_MIPS_ARCH_32R2 = 0x70000000;
constexpr int EF_MIPS_ARCH_64R2 = 0x80000000;
constexpr int EF_MIPS_ABI_EABI32 = 0x00003000;
constexpr int EF_MIPS_ABI_EABI64 = 0x00004000;

constexpr int R_386_NONE = 0;
constexpr int R_386_32 = 1;
constexpr int R_386_PC32 = 2;
constexpr int R_386_GOT32 = 3;
constexpr int R_386_PLT32 = 4;
constexpr int R_386_COPY = 5;
constexpr int R_386_GLOB_DAT = 6;
constexpr int R_386_JUMP_SLOT = 7;
constexpr int R_386_RELATIVE = 8;
constexpr int R_386_GOTOFF = 9;
constexpr int R_386_GOTPC = 10;

constexpr int R_X86_64_NONE = 0;
constexpr int R_X86_64_PC32 = 2;
constexpr int R_X86_64_COPY = 5;
constexpr int R_X86_64_GLOB_DAT = 6;
constexpr int R_X86_64_JUMP_SLOT = 7;
constexpr int R_X86_64_RELATIVE = 8;
constexpr int R_X86_64_DTPMOD64 = 16;
constexpr int R_X86_64_DTPOFF64 = 17;
constexpr int R_X86_64_TPOFF64 = 18;
constexpr int R_X86_64_TLSDESC = 36;
constexpr int R_X86_64_IRELATIVE = 37;
constexpr int R_X86_64_RELATIVE64 = 38;

constexpr int R_ARM_NONE = 0;
constexpr int R_ARM_PC24 = 1;
constexpr int R_ARM_ABS32 = 2;
constexpr int R_ARM_REL32 = 3;
constexpr int R_ARM_PC13 = 4;
constexpr int R_ARM_ABS16 = 5;
constexpr int R_ARM_ABS12 = 6;
constexpr int R_ARM_THM_ABS5 = 7;
constexpr int R_ARM_ABS8 = 8;
constexpr int R_ARM_SBREL32 = 9;
constexpr int R_ARM_THM_PC22 = 10;
constexpr int R_ARM_THM_PC8 = 11;
constexpr int R_ARM_AMP_VCALL9 = 12;
constexpr int R_ARM_SWI24 = 13;
constexpr int R_ARM_THM_SWI8 = 14;
constexpr int R_ARM_XPC25 = 15;
constexpr int R_ARM_THM_XPC22 = 16;
constexpr int R_ARM_TLS_DTPMOD32 = 17;
constexpr int R_ARM_TLS_DTPOFF32 = 18;
constexpr int R_ARM_TLS_TPOFF32 = 19;
constexpr int R_ARM_COPY = 20;
constexpr int R_ARM_GLOB_DAT = 21;
constexpr int R_ARM_JUMP_SLOT = 22;
