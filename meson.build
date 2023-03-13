project(
  'mcc', 'c'
)

# Keep this in alphabetical order please!
sources = [
  'src/alloc.c',
  'src/args.c',
  'src/emit-llvm/emit.c',
  'src/emit-llvm/fn_decl.c',
  'src/emit-llvm/fn_def.c',
  'src/emit-llvm/gen_type.c',
  'src/emit-llvm/module.c',
  'src/errmsg.c',
  'src/file.c',
  'src/help.c',
  'src/mangle.c',
  'src/mcc.c',
  'src/mcc_compile.c',
  'src/mcc_link.c',
  'src/mcc_run.c',
  'src/p_block.c',
  'src/p_fn.c',
  'src/p_match.c',
  'src/p_module.c',
  'src/p_node.c',
  'src/p_statement.c',
  'src/p_type.c',
  'src/p_use.c',
  'src/p_utils.c',
  'src/p_value.c',
  'src/parse.c',
  'src/paths.c',
  'src/settings.c',
  'src/strlist.c',
  'src/targets.c',
  'src/term.c',
  'src/tokenize.c',
  'src/type.c',
]

includes = include_directories('include')
lib_llvm = declare_dependency(link_args: '-lLLVM')
libdir = get_option('prefix') / get_option('libdir')
bindir = get_option('prefix') / get_option('bindir')

link_args = ['-fsanitize=address']
c_args = ['-Wall', '-Wextra', '-fsanitize=address', '-DOPT_ALLOC_SLAB_INFO=1']

exe = executable(
  'mcc', sources,
  include_directories: includes,
  c_args: c_args,
  link_args: link_args,
  dependencies: lib_llvm,
  install: true,
  install_dir: bindir
)
