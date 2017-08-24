project(
	'libsodium', 'c',
	version : '1.0.14',
	license : 'ISC',
	default_options : ['c_std=c99'],
)

version_conf = configuration_data()
version_conf.set('SODIUM_LIBRARY_VERSION_MAJOR', 9)
version_conf.set('SODIUM_LIBRARY_VERSION_MINOR', 6)
version_conf.set_quoted('SODIUM_VERSION_STRING', meson.project_version())

dll_version = 8
sodium_library_version = '19.4.0'

compiler = meson.get_compiler('c')
add_project_arguments('-DCONFIGURED__=1', language : 'c')

add_project_arguments('-D__EXTENSIONS__=1', language : 'c')
add_project_arguments('-D_ALL_SOURCE=1', language : 'c')
add_project_arguments('-D_GNU_SOURCE=1', language : 'c')
add_project_arguments('-D_POSIX_PTHREAD_SEMANTICS=1', language : 'c')
add_project_arguments('-D_TANDEM_SOURCE=1', language : 'c')

is_nativeclient = compiler.get_define('__native_client__') != ''
is_pnacl = is_nativeclient and compiler.get_define('__pnacl__') != ''
is_emscripten = compiler.get_define('__EMSCRIPTEN__') != ''
is_win_posix = false # TODO: Detection

# TODO: Safecode

enable_asm = get_option('asm')
if is_nativeclient or is_emscripten
	enable_asm = false
	message('Compiling to Emscripten/Native Client - asm implementations disabled')
endif

if get_option('minimal')
	version_conf.set('SODIUM_LIBRARY_MINIMAL', 1, description: 'Minimal build')
endif

if get_option('pthread')
	threads = dependency('threads')
	add_project_arguments('-DHAVE_PTHREAD=1', language : 'c')
else
	threads = declare_dependency()
endif

# Set optimization flags
if get_option('optimize')
	if compiler.has_argument('-Ofast')
		add_project_arguments('-Ofast', language : 'c')
	endif

	if compiler.has_argument('-march=native')
		add_project_arguments('-march=native', language : 'c')
	endif
endif

# _FORTIFY_SOURCE support
if compiler.get_define('_FORTIFY_SOURCE') == ''
	if compiler.has_argument('-D_FORTIFY_SOURCE=2')
		add_project_arguments('-D_FORTIFY_SOURCE=2', language : 'c')
	endif
endif

if get_option('pie') and not is_win_posix and compiler.has_argument('-fPIE')
	add_project_arguments('-fPIE', language : 'c')
endif

wanted_arguments = [
	'-fvisibility=hidden',
	'-fno-strict-aliasing',
	'-Wwrite-strings',
	'-Wdiv-by-zero',
	'-Wsometimes-uninitialized',
]
foreach wanted : wanted_arguments
	if compiler.has_argument(wanted)
		add_project_arguments(wanted, language : 'c')
	endif
endforeach

wanted_arguments = [
	'-Wl,-z,relro',
	'-Wl,-z,now',
	'-Wl,-z,noexecstack',
]
foreach wanted : wanted_arguments
	if compiler.has_argument(wanted)
		add_project_link_arguments(wanted, language : 'c')
	endif
endforeach

if is_win_posix
	wanted_arguments = ['-Wl,--dynamicbase', '-Wl,--high-entropy-va', '-Wl,--nxcompat']
	foreach wanted : wanted_arguments
		if compiler.has_argument(wanted)
			add_project_arguments(wanted, language : 'c')
		endif
	endforeach
endif

supported_overflow_args = compiler.first_supported_argument(['-fno-strict-overflow', '-fwrapv'])
add_project_arguments(supported_overflow_args, language : 'c')

enable_ssp = get_option('ssp')
if enable_ssp and not is_win_posix and compiler.has_argument('-fstack-protector')
	# TODO: Test linker against -fstack-protector
	add_project_arguments('-fstack-protector', language : 'c')
endif

cwflags = []
cwflags_wanted = [
	'-Wall',
	'-Wextra',
	'-Wbad-function-cast',
	'-Wcast-align',
	'-Wcast-qual',
	'-Wduplicated-cond',
	'-Wfloat-equal',
	'-Wformat=2',
	'-Wmissing-declarations',
	'-Wmissing-prototypes',
	'-Wnested-externs',
	'-Wno-type-limits',
	'-Wno-unknown-pragmas',
	'-Wnormalized=id',
	'-Wnull-dereference',
	'-Wold-style-declaration',
	'-Wpointer-arith',
	'-Wredundant-decls',
	'-Wshorten-64-to-32',
	'-Wstrict-prototypes',
	'-Wswitch-enum',
	'-Wvariable-decl',
]
if compiler.get_id() == 'clang'
	cwflags_wanted += '-Wno-unknown-warning-option'
endif
foreach wanted : cwflags_wanted
	if compiler.has_argument(wanted)
		cwflags += wanted
	endif
endforeach
cwflags = declare_dependency(compile_args : cwflags)

# TODO: AX_CHECK_CATCHABLE_SEGV

if not is_emscripten and not is_pnacl
	mmx = declare_dependency()
	compiler_args = []
	if compiler.has_argument('-mmmx')
		mmx = declare_dependency(compile_args : '-mmmx')
		compiler_args += '-mmmx'
	endif
	has_mmx = compiler.compiles('''
#pragma GCC target("mmx")
#include <mmintrin.h>
int main(){
	__m64 x = _mm_setzero_si64();
}
	''', args : compiler_args)
	if has_mmx
		message('MMX is available')
		add_project_arguments('-DHAVE_MMINTRIN_H=1', language : 'c')
	else
		message('MMX is NOT available')
	endif

	sse2 = declare_dependency()
	compiler_args = []
	if compiler.has_argument('-msse2')
		sse2 = declare_dependency(compile_args : '-msse2')
		compiler_args += '-msse2'
	endif
	has_sse2 = compiler.compiles('''
#pragma GCC target("sse2")
#ifndef __SSE2__
# define __SSE2__
#endif
#include <emmintrin.h>
int main(){
	__m128d x = _mm_setzero_pd();
	__m128i z = _mm_srli_epi64(_mm_setzero_si128(), 26);
}
	''', args: compiler_args)
	if has_sse2
		message('SSE2 is available')
		add_project_arguments('-DHAVE_EMMINTRIN_H=1', language : 'c')
	else
		message('SSE2 is NOT available')
	endif

	sse3 = declare_dependency()
	compiler_args = []
	if compiler.has_argument('-msse3')
		sse3 = declare_dependency(compile_args : '-msse3')
		compiler_args += '-msse3'
	endif
	has_sse3 = compiler.compiles('''
#pragma GCC target("sse3")
#include <pmmintrin.h>
int main(){
	__m128 x = _mm_addsub_ps(_mm_cvtpd_ps(_mm_setzero_pd()), _mm_cvtpd_ps(_mm_setzero_pd()));
}
	''', args: compiler_args)
	if has_sse3
		message('SSE3 is available')
		add_project_arguments('-DHAVE_PMMINTRIN_H=1', language : 'c')
	else
		message('SSE3 is NOT available')
	endif

	ssse3 = declare_dependency()
	compiler_args = []
	if compiler.has_argument('-mssse3')
		ssse3 = declare_dependency(compile_args : '-mssse3')
		compiler_args += '-mssse3'
	endif
	has_ssse3 = compiler.compiles('''
#pragma GCC target("ssse3")
#include <tmmintrin.h>
int main(){
	 __m64 x = _mm_abs_pi32(_m_from_int(0));
}
	''', args : compiler_args)
	if has_ssse3
		message('SSSE3 is available')
		add_project_arguments('-DHAVE_TMMINTRIN_H=1', language : 'c')
	else
		message('SSSE3 is NOT available')
	endif

	sse4_1 = declare_dependency()
	compiler_args = []
	if compiler.has_argument('-msse4.1')
		sse4_1 = declare_dependency(compile_args : '-msse4.1')
		compiler_args += '-msse4.1'
	endif
	has_sse4_1 = compiler.compiles('''
#pragma GCC target("sse4.1")
#include <smmintrin.h>
int main(){
	 __m128i x = _mm_minpos_epu16(_mm_setzero_si128());
}
	''', args : compiler_args)
	if has_sse4_1
		message('SSE4.1 is available')
		add_project_arguments('-DHAVE_SMMINTRIN_H=1', language : 'c')
	else
		message('SSE4.1 is NOT available')
	endif

	avx = declare_dependency()
	if not is_nativeclient
		compiler_args = []
		if compiler.has_argument('-mavx')
			avx = declare_dependency(compile_args : '-mavx')
			compiler_args += '-mavx'
		endif
		has_avx = compiler.compiles('''
#pragma GCC target("avx")
#include <immintrin.h>
int main(){
	_mm256_zeroall();
}
	''', args : compiler_args)
		if has_avx
			message('AVX is available')
			add_project_arguments('-DHAVE_AVXINTRIN_H=1', language : 'c')
		else
			message('AVX is NOT available')
		endif
	endif

	avx2 = declare_dependency()
	if not is_nativeclient
		compiler_args = []
		if compiler.has_argument('-mavx2')
			avx2 = declare_dependency(compile_args : '-mavx2')
			compiler_args += '-mavx2'
		endif
		has_avx2 = compiler.compiles('''
#pragma GCC target("avx2")
#include <immintrin.h>
int main(){
	__m256 x = _mm256_set1_ps(3.14);
	__m256 y = _mm256_permutevar8x32_ps(x, _mm256_set1_epi32(42));
	return _mm256_movemask_ps(_mm256_cmp_ps(x, y, _CMP_NEQ_OQ));
}
	''', args : compiler_args)
		if has_avx2
			message('AVX2 is available')
			add_project_arguments('-DHAVE_AVX2INTRIN_H=1', language : 'c')

			if not compiler.compiles('''
#pragma GCC target("avx2")
#include <immintrin.h>
int main(){
	__m256i y = _mm256_broadcastsi128_si256(_mm_setzero_si128());
}
	''', args : compiler_args)
				add_project_arguments('-D_mm256_broadcastsi128_si256=_mm_broadcastsi128_si256', language : 'c')
			endif
		else
			message('AVX2 is NOT available')
		endif
	endif

	aes = declare_dependency()
	if not is_nativeclient
		compiler_args = []
		if compiler.has_argument('-maes')
			aes = declare_dependency(compile_args : '-maes')
			compiler_args += '-maes'
		endif
		pclmul = declare_dependency()
		if compiler.has_argument('-mpclmul')
			pclmul = declare_dependency(compile_args : '-mpclmul')
			compiler_args += '-mpclmul'
		endif
		has_aes = compiler.compiles('''
#pragma GCC target("aes")
#pragma GCC target("pclmul")
#include <wmmintrin.h>
int main(){
	__m128i x = _mm_aesimc_si128(_mm_setzero_si128());
	__m128i y = _mm_clmulepi64_si128(_mm_setzero_si128(), _mm_setzero_si128(), 0);
}
		''', args : compiler_args)
		if has_aes
			message('AESNI and PCLMULQDQ are available')
			add_project_arguments('-DHAVE_WMMINTRIN_H=1', language : 'c')
		else
			message('AESNI and PCLMULQDQ are NOT available')
		endif
	endif
endif

if compiler.has_header('sys/mman.h')
	add_project_arguments('-DHAVE_SYS_MMAN_H=1', language : 'c')
endif
if compiler.has_header('intrin.h')
	add_project_arguments('-DHAVE_INTRIN_H=1', language : 'c')

	has_xgetbv = compiler.links('''#include <intrin.h>
int main() { (void) _xgetbv(0) }''')
	if has_xgetbv
		add_project_arguments('-DHAVE__XGETBV=1', language : 'c')
		message('_xgetbv() is available')
	else
		message('_xgetbv() is NOT available')
	endif
endif

if host_machine.endian() == 'big'
	add_project_arguments('-DNATIVE_BIG_ENDIAN=1', language : 'c')
else
	add_project_arguments('-DNATIVE_LITTLE_ENDIAN=1', language : 'c')
endif

if compiler.get_id() == 'clang' and host_machine.cpu_family() == 'x86'
	# clang does not properly handle the 128-bit type on 32-bit systems
	can_use_128_bits = false
elif host_machine.endian() != 'little'
	# libsodium currently expects a little endian CPU for the 128-bit type
	can_use_128_bits = false
elif is_emscripten
	# emscripten currently supports only shift operations on integers larger than 64 bits
	can_use_128_bits = false
else
	can_use_128_bits = compiler.compiles('''
#if !defined(__GNUC__) && !defined(__SIZEOF_INT128__)
# error mode(TI) is a gcc extension, and __int128 is not available
#endif
#include <stddef.h>
#include <stdint.h>
#if defined(__SIZEOF_INT128__)
typedef unsigned __int128 uint128_t;
#else
typedef unsigned uint128_t __attribute__((mode(TI)));
#endif
void fcontract(uint128_t *t) {
  *t += 0x8000000000000 - 1;
}
int main(){
	(void) fcontract;
}
		''')
endif
if can_use_128_bits
	add_project_arguments('-DHAVE_TI_MODE=1', language : 'c')
	message('gcc TI mode is available')
else
	message('gcc TI mode is NOT available')
endif

can_use_x86_64_asm = false
can_use_avx = false
can_use_cpuid = false

if enable_asm
	can_use_x86_64_asm = compiler.compiles('''
int main(){
	#if defined(__amd64) || defined(__amd64__) || defined(__x86_64__)
	# if defined(__CYGWIN__) || defined(__MINGW32__) || defined(__MINGW64__) || defined(_WIN32) || defined(_WIN64)
	#  error Windows x86_64 calling conventions are not supported yet
	# endif
	/* neat */
	#else
	# error !x86_64
	#endif
	unsigned char i = 0, o = 0, t;
	__asm__ __volatile__ ("pxor %%xmm12, %%xmm6 \n"
						"movb (%[i]), %[t] \n"
						"addb %[t], (%[o]) \n"
						: [t] "=&r"(t)
						: [o] "D"(&o), [i] "S"(&i)
						: "memory", "flags", "cc");
}
''')
	if can_use_x86_64_asm
		add_project_arguments('-DHAVE_AMD64_ASM=1', language : 'c')
		message('x86_64 asm code can be used')
	else
		message('x86_64 asm code CANNOT be used')
	endif

	can_use_avx = compiler.compiles('''
int main(){
	#if defined(__amd64) || defined(__amd64__) || defined(__x86_64__)
	# if defined(__CYGWIN__) || defined(__MINGW32__) || defined(__MINGW64__) || defined(_WIN32) || defined(_WIN64)
	#  error Windows x86_64 calling conventions are not supported yet
	# endif
	/* neat */
	#else
	# error !x86_64
	#endif
	__asm__ __volatile__ ("vpunpcklqdq %xmm0,%xmm13,%xmm0");
}
''')
	if can_use_avx
		add_project_arguments('-DHAVE_AVX_ASM=1', language : 'c')
		message('AVX opcodes are supported')
	else
		message('AVX opcodes are NOT supported')
	endif
endif

if enable_asm or (is_nativeclient and host_machine.cpu_family() == 'x86_64')
	can_use_cpuid = compiler.compiles('''
int main(){
unsigned int cpu_info[4];
__asm__ __volatile__ ("xchgl %%ebx, %k1; cpuid; xchgl %%ebx, %k1" :
					"=a" (cpu_info[0]), "=&r" (cpu_info[1]),
					"=c" (cpu_info[2]), "=d" (cpu_info[3]) :
					"0" (0U), "2" (0U));
}
	''')
	if can_use_cpuid
		add_project_arguments('-DHAVE_CPUID=1', language : 'c')
		message('cpuid instruction is available')
	else
		message('cpuid instruction is NOT available')
	endif
endif

if enable_asm or is_nativeclient
	can_use_private_extern = compiler.links('''
int main(){
	__asm__ __volatile__ (".private_extern dummy_symbol \n"
						".private_extern _dummy_symbol \n"
						".globl dummy_symbol \n"
						".globl _dummy_symbol \n"
						"dummy_symbol: \n"
						"_dummy_symbol: \n"
						"    nop \n"
	);
}
	''')
	if can_use_private_extern
		add_project_arguments('-DASM_HIDE_SYMBOL=.private_extern', language : 'c')
	else
		can_use_hidden_asm = compiler.links('''
int main(){
	__asm__ __volatile__ (".hidden dummy_symbol \n"
						".hidden _dummy_symbol \n"
						".globl dummy_symbol \n"
						".globl _dummy_symbol \n"
						"dummy_symbol: \n"
						"_dummy_symbol: \n"
						"    nop \n"
	);
}
	''')
		if can_use_hidden_asm
			add_project_arguments('-DASM_HIDE_SYMBOL=.hidden', language : 'c')
		else
			message('Unable to reliably tag symbols as private')
		endif
	endif
endif

can_use_weak_symbols = compiler.links('''
#if !defined(__ELF__) && !defined(__APPLE_CC__)
# error Support for weak symbols may not be available
#endif
__attribute__((weak)) void __dummy(void *x) { }
void f(void *x) { __dummy(x); }
int main(){}
''')
if can_use_weak_symbols
	add_project_arguments('-DHAVE_WEAK_SYMBOLS=1', language : 'c')
	message('Weak symbols are supported')
else
	message('Weak symbols are NOT supported')
endif

can_use_atomic_ops = compiler.links('''
int main(){
	static volatile int _sodium_lock;
	__sync_lock_test_and_set(&_sodium_lock, 1);
	__sync_lock_release(&_sodium_lock);
}
''')
if can_use_atomic_ops
	add_project_arguments('-DHAVE_ATOMIC_OPS=1', language : 'c')
	message('Atomic operations are supported')
else
	message('Atomic operations are NOT supported')
endif

check_functions = [
	'posix_memalign',
	'getpid',
]

if not is_emscripten
	check_functions += [
		'arc4random',
		'arc4random_buf',
		'mmap',
		'mlock',
		'madvise',
		'mprotect',
		'memset_s',
		'explicit_bzero',
		'nanosleep',
	]
endif

foreach func : check_functions
	if compiler.has_function(func)
		add_project_arguments('-DHAVE_' + func.to_upper() + '=1', language : 'c')
	endif
endforeach

# TODO: Emscripten LDFlags (use target file?)

# TODO:
#src/libsodium/include/Makefile
#                 src/libsodium/include/sodium/version.h
#                 test/default/Makefile
#                 test/Makefile

subdir('src/libsodium')
