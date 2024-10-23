project(
  'duckdb',
  'cpp',
  license: 'mit',
  version: '1.1.2',
  meson_version: '>=0.48.0',
)

sources = files('duckdb.cpp')
headers = files('duckdb.hpp', 'duckdb.h', 'duckdb_extension.h')

libduckdb = library(
  'duckdb',
  sources,
  install: true,
  gnu_symbol_visibility: 'hidden',
)

install_headers(headers)

duckdb_dep = declare_dependency(
  include_directories: include_directories('.'),
  link_with: libduckdb,
)
