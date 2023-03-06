project(
  'mcc', 'c'
)

# Keep this in alphabetical order please!
sources = [
  'src/args.c',
  'src/errmsg.c',
  'src/help.c',
  'src/mcc.c',
  'src/mcc_compile.c',
  'src/mcc_link.c',
  'src/mcc_run.c',
  'src/paths.c',
  'src/settings.c',
  'src/strlist.c',
  'src/term.c',
]

includes = include_directories('include')
lib_llvm = declare_dependency(link_args: '-lLLVM')
libdir = get_option('prefix') / get_option('libdir')
bindir = get_option('prefix') / get_option('bindir')

link_args = ['-fsanitize=address']
c_args = ['-fsanitize=address', '-DOPT_ALLOC_SLAB_INFO=1']

exe = executable(
  'mcc', sources,
  include_directories: includes,
  c_args: c_args,
  link_args: link_args,
  dependencies: lib_llvm,
  install: true,
  install_dir: bindir
)
