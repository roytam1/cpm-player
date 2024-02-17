/*
	CP/M Player for Win32 console

	Author : Takeda.Toshiya
	Date   : 2009.05.05-
*/

// variable scope of 'for' loop for microsoft visual c++ 6.0 and embedded visual c++ 4.0
#if defined(_MSC_VER) && (_MSC_VER == 1200)
#define for if(0);else for
#endif

// disable warnings C4189, C4995 and C4996 for microsoft visual c++ 2005
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#pragma warning( disable : 4819 )
#pragma warning( disable : 4995 )
#pragma warning( disable : 4996 )
#endif

// type definition
#ifndef uint8
typedef unsigned char uint8;
#endif
#ifndef uint16
typedef unsigned short uint16;
#endif
#ifndef uint32
typedef unsigned int uint32;
#endif
#ifndef uint64
#ifdef _MSC_VER
typedef unsigned __int64 uint64;
#else
typedef unsigned long long uint64;
#endif
#endif

#ifndef int8
typedef signed char int8;
#endif
#ifndef int16
typedef signed short int16;
#endif
#ifndef int32
typedef signed int int32;
#endif
#ifndef int64
#ifdef _MSC_VER
typedef signed __int64 int64;
#else
typedef signed long long int64;
#endif
#endif

typedef union {
#ifdef _BIG_ENDIAN
	struct {
		uint8 h3, h2, h, l;
	} b;
	struct {
		int8 h3, h2, h, l;
	} sb;
	struct {
		uint16 h, l;
	} w;
	struct {
		int16 h, l;
	} sw;
#else
	struct {
		uint8 l, h, h2, h3;
	} b;
	struct {
		uint16 l, h;
	} w;
	struct {
		int8 l, h, h2, h3;
	} sb;
	struct {
		int16 l, h;
	} sw;
#endif
	uint32 d;
	int32 sd;
} pair;

/* ----------------------------------------------------------------------------
	CP/M virtual machine
---------------------------------------------------------------------------- */

#define MAX_DRIVES	1
#define MAX_FILES	16
#define TPA_BASE	0x100
#define CPP_BASE	0xfd00
#define BDOS_BASE	0xfe00
#define BDOS_BASE_2	0xfe80
#define BIOS_BASE	0xff00
#define BIOS_BASE_2	0xff80

uint8 mem[0x10000];
uint8 user_id;
uint8 default_drive;
uint16 login_drive;
uint8 delimiter;
uint8 read_only[16];
uint16 dma_addr;
uint8 find_files[256][12];
int find_num, find_idx;
FILE *file_ptr[MAX_FILES];
char file_path[MAX_FILES][256];

void cpm_bios(int num);
void cpm_bdos();

void cpm_create_path(int drive, uint8* src, char* dest);
FILE* cpm_get_file_ptr(char* path);
void cpm_close_file(char* path);
int cpm_get_file_size(char* path);

/* ----------------------------------------------------------------------------
	Console
---------------------------------------------------------------------------- */

#define SCR_BUF_SIZE	1200

CPINFO cpinfo;
HANDLE hStdout;
CHAR_INFO scr_buf[SCR_BUF_SIZE][80];
COORD scr_buf_size;
COORD scr_buf_pos;
WORD wPrevAttributes;
int prev_stdin_mode, prev_stdout_mode;

void cons_init();
void cons_finish();
int cons_kbhit();
int cons_getch();
int cons_getche();
void cons_putch(UINT8 data);

/* ----------------------------------------------------------------------------
	Z80 (MAME 0.145)
---------------------------------------------------------------------------- */

#define CF	0x01
#define NF	0x02
#define PF	0x04
#define VF	PF
#define XF	0x08
#define HF	0x10
#define YF	0x20
#define ZF	0x40
#define SF	0x80

#define PCD	pc.d
#define PC	pc.w.l

#define SPD 	sp.d
#define SP	sp.w.l

#define AFD 	af.d
#define AF	af.w.l
#define A	af.b.h
#define F	af.b.l

#define BCD 	bc.d
#define BC	bc.w.l
#define B	bc.b.h
#define C	bc.b.l

#define DED 	de.d
#define DE	de.w.l
#define D	de.b.h
#define E	de.b.l

#define HLD 	hl.d
#define HL	hl.w.l
#define H	hl.b.h
#define L	hl.b.l

#define IXD 	ix.d
#define IX	ix.w.l
#define HX	ix.b.h
#define LX	ix.b.l

#define IYD 	iy.d
#define IY	iy.w.l
#define HY	iy.b.h
#define LY	iy.b.l

#define AF2	af2.w.l
#define A2	af2.b.h
#define F2	af2.b.l

#define BC2	bc2.w.l
#define B2	bc2.b.h
#define C2	bc2.b.l

#define DE2	de2.w.l
#define D2	de2.b.h
#define E2	de2.b.l

#define HL2	hl2.w.l
#define H2	hl2.b.h
#define L2	hl2.b.l

#define WZD	wz.d
#define WZ	wz.w.l
#define WZ_H	wz.b.h
#define WZ_L	wz.b.l

uint8 SZ[256];		/* zero and sign flags */
uint8 SZ_BIT[256];	/* zero, sign and parity/overflow (=zero) flags for BIT opcode */
uint8 SZP[256];		/* zero, sign and parity flags */
uint8 SZHV_inc[256];	/* zero, sign, half carry and overflow flags INC r8 */
uint8 SZHV_dec[256];	/* zero, sign, half carry and overflow flags DEC r8 */

uint8 SZHVC_add[2 * 256 * 256];
uint8 SZHVC_sub[2 * 256 * 256];

uint16 prevpc;
pair pc, sp, af, bc, de, hl, ix, iy, wz;
pair af2, bc2, de2, hl2;
uint8 I, R, R2;
uint32 ea;

bool halt;
uint8 im, iff1, iff2;
bool after_ei, after_ldair;

inline uint8 RM8(uint32 addr);
inline void WM8(uint32 addr, uint8 val);
inline void RM16(uint32 addr, pair *r);
inline void WM16(uint32 addr, pair *r);
inline uint8 FETCHOP();
inline uint8 FETCH8();
inline uint32 FETCH16();
inline uint8 IN8(uint32 addr);
inline void OUT8(uint32 addr, uint8 val);

inline uint8 INC(uint8 value);
inline uint8 DEC(uint8 value);

inline uint8 RLC(uint8 value);
inline uint8 RRC(uint8 value);
inline uint8 RL(uint8 value);
inline uint8 RR(uint8 value);
inline uint8 SLA(uint8 value);
inline uint8 SRA(uint8 value);
inline uint8 SLL(uint8 value);
inline uint8 SRL(uint8 value);

inline uint8 RES(uint8 bit, uint8 value);
inline uint8 SET(uint8 bit, uint8 value);

void OP_CB(uint8 code);
void OP_XY(uint8 code);
void OP_DD(uint8 code);
void OP_FD(uint8 code);
void OP_ED(uint8 code);
void OP(uint8 code);
