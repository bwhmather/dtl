project('dtl', ['c', 'cpp'], default_options: [
  'cpp_std=c++20',
])

cc = meson.get_compiler('cpp')

m_dep = cc.find_library('m', required : false)
arrow_dep = dependency('arrow')
duckdb_dep = dependency('duckdb')
parquet_dep = dependency('parquet')
uuid_dep = dependency('uuid')
xxhash_dep = dependency('libxxhash')

dependencies = [m_dep, arrow_dep, duckdb_dep, parquet_dep, uuid_dep, xxhash_dep]

includes = include_directories('src')

### Library ###

bison = find_program('bison')

parser_source = custom_target(
  'parser',
  input: ['src/dtl-parser.y'], output: ['dtl-parser.c'],
  command: [bison, '-o', '@OUTPUT@', '@INPUT@'],
)

lib_sources = [
  'src/dtl-ast.c',
  'src/dtl-ast-to-ir.c',
  'src/dtl-bool-array.c',
  'src/dtl-double-array.c',
  'src/dtl-dtype.c',
  'src/dtl-error.c',
  'src/dtl-eval.c',
  'src/dtl-index-array.c',
  'src/dtl-int64-array.c',
  'src/dtl-io.c',
  'src/dtl-io-duckdb.c',
  'src/dtl-io-filesystem.cpp',
  'src/dtl-ir.c',
  'src/dtl-ir-viz.c',
  'src/dtl-location.c',
  'src/dtl-manifest.c',
  'src/dtl-schema.c',
  'src/dtl-string-array.c',
  'src/dtl-string-interner.c',
  'src/dtl-tokenizer.c',
  'src/dtl-value.c',
  parser_source,
]

lib = both_libraries(
  'dtl', lib_sources,
  include_directories : includes,
  dependencies : dependencies,
  install : true,
)

### Executable ###

exe = executable(
  'dtl',
  'src/main.c',
  include_directories : includes,
  dependencies : dependencies,
  link_with : lib,
  install : true,
)

### Tests ###

test_includes = include_directories('src', 'tests')

test_suites = {
  'bool-array': [
    'not',
  ],
  'string-interner': [
    'intern',
    'reallocate',
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
      'tests/' + suite + '/test-' + test_name + '.c',
      include_directories : test_includes,
      link_with : lib,
      install : false,
    )
    test(suite + '-' + test_name, test_exe, timeout: 10)
  endforeach
endforeach

py_test_suites = {
  'end-to-end': [
    'add-expression',
    'basic',
    'duplicate-columns',
    'equal',
    'export-twice',
    'full-outer-join',
    'simple-join',
    'less-than',
    'rename-columns',
    'split-columns',
    'subset-columns',
  ],
  'lint': [
    'exact-includes',
    'unused-symbols',
    'no-circular-includes',
    'source-and-header-orders-match',
    'source-and-header-contents-match',
  ]
}

pymod = import('python')
python = pymod.find_installation('python3')

foreach suite, tests : py_test_suites
  foreach test_name : tests
    test(
      suite + '-' + test_name,
      python,
      args: [
        meson.current_source_dir() +
        '/tests/' + suite +
        '/test_' + test_name.replace('-', '_') + '.py',
      ],
      env: {
        'DTL': exe.full_path(),
        'MESON_BUILD_ROOT': meson.project_build_root(),
        'MESON_SOURCE_ROOT': meson.project_source_root(),
      },
      depends: [exe],
      timeout: 120,
    )

  endforeach
endforeach
