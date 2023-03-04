project(
  'mcc', 'c', version: '0.9',
  default_options: [
    'prefix=/usr',
    'libdir=lib/mocha',
    'bindir=bin'
  ]
)

sources = [
  # Main
  'src/mcc.c',

  # Compiler
  'src/tokenize.c',
  'src/parser.c',
  'src/parser/is.c',
  'src/parser/value.c',
  'src/parser/use.c',
  'src/parser/call.c',
  'src/parser/dump.c',
  'src/parser/fn.c',
  'src/parser.c',
  'src/type.c',
  'src/alloc.c',
  'src/keyword.c',
  'src/emit.c',
  'src/module.c',
  'src/mangle.c',
  'src/build.c',

  # Utils
  'src/utils/utils.c',
  'src/utils/error.c',
  'src/utils/file.c'
]

includes = include_directories('include')
llvm = declare_dependency(link_args: '-lLLVM')
libdir = get_option('prefix') / get_option('libdir')
bindir = get_option('prefix') / get_option('bindir')

c_args = [
  '-Wall', '-Wextra',
  '-DMCC_ROOT="' + libdir + '"'
]

link_args = []

if get_option('buildtype') == 'debug'
  c_args += ['-O0', '-ggdb', '-fsanitize=address',
    '-DOPT_ALLOC_SLAB_INFO=1', '-DOPT_DEBUG_INFO=1', '-DOPT_ASAN=1',
  ]
  link_args += '-fsanitize=address'
else
  c_args += '-O3'
endif

exe = executable(
  'mcc', sources,
  include_directories: includes,
  c_args: c_args,
  link_args: link_args,
  dependencies: llvm,
  install: true,
  install_dir: bindir
)
