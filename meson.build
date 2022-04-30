project('dtl', 'cpp', default_options: [
    'cpp_std=c++20',
])

cc = meson.get_compiler('cpp')

m_dep = cc.find_library('m', required : false)
arrow_dep = dependency('arrow')
parquet_dep = dependency('parquet')

dependencies = [m_dep, arrow_dep, parquet_dep]

includes = include_directories('src')


### Library ###

bison = find_program('bison')

parser_source = custom_target(
    'parser',
    input: ['src/dtl-parser.y'], output: ['dtl-parser.cpp'],
    command: [bison, '-o', '@OUTPUT@', '@INPUT@'],
)

lib_sources = [
  'src/dtl-ast.cpp',
  'src/dtl-ast-find-imports.cpp',
  'src/dtl-ast-to-ir.cpp',
  'src/dtl-ast-visit-children.cpp',
  'src/dtl-io-filesystem.cpp',
  'src/dtl-ir.cpp',
  'src/dtl-tokenizer.cpp',
  'src/dtl-tokens.cpp',
  parser_source,
]

lib = both_libraries(
  'dtl', lib_sources,
  include_directories : includes,
  dependencies : dependencies,
  install : true,
)


### Executable ###

test_exe = executable(
  'dtl',
  'src/main.cpp',
  include_directories : includes,
  dependencies : dependencies,
  link_with : lib,
  install : true,
)


### Tests ###

test_includes = include_directories('src', 'tests')

test_suites = {
  'ast': [
    'find-imports',
  ],
  'parser': [
    'basic',
  ],
  'tokenizer': [
    'all-keywords',
    'basic',
    'concatenated-keyword',
    'empty',
    'keyword',
    'linebreak',
  ],
}

foreach suite, tests : test_suites
  foreach test_name : tests
    test_exe = executable(
      'test-' + suite + '-' + test_name,
      'tests/' + suite + '/test-' + test_name + '.cpp',
      include_directories : test_includes,
      link_with : lib,
      install : false,
    )
    test(suite + '-' + test_name, test_exe, timeout: 10)
  endforeach
endforeach
