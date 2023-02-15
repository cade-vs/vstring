project(
  'vstring',
  'cpp',
  version: '0.0.0999',
  meson_version : '>= 0.48.0',
  default_options: [
    'warning_level=3',
    'b_lto=true',
    'b_pie=true'
    ]
  )

cxx = meson.get_compiler('cpp')
extra_flags = [
  '-fno-common',
  '-Wshadow',
  '-Wformat-security',
  '-Wformat-overflow=2',
  '-Wdate-time',
  '-D_FORTIFY_SOURCE=2',
  '-DVSTRING_VERSION="@0@"'.format(meson.project_version())
]

add_project_arguments(cxx.get_supported_arguments(extra_flags), language: 'cpp')

src = [
  'vref.cpp',
  'vstring.cpp',
  'vstrlib.cpp',
  'vstruti.cpp',
  'wstring.cpp',
  'wstrlib.cpp',
]

deps = [
  dependency('libpcre2-32'),
  dependency('libpcre2-8')
]

buildtarget = library(
  meson.project_name(),
  src,
  dependencies : deps,
  version : meson.project_version(),
  soversion : '0',
  install: not meson.is_subproject())

# How to use in a superproject and other info
# https://mesonbuild.com/Subprojects.html
dep_vstring = declare_dependency(
  link_with : buildtarget,
  include_directories('.')
)

if not meson.is_subproject()
  pkg = import('pkgconfig')
  pkg.generate(
    buildtarget,
    description : 'C++ string manipulation and handling library with Unicode support')

  install_headers('vstring.h', 'vstrlib.h')
  install_data(files(
    'AUTHORS',
    'COPYING',
    'LICENSE',
    'README',
    'README.md',
    'TODO'
    ),
    install_dir : get_option('docdir'))
endif

test_cases = [
  'test',
  'wtest',
  ]

foreach case : test_cases
  exe = executable(
    case,
    case + '.cpp',
    dependencies: [dep_vstring]
    )
  test('test_' + case, exe)
endforeach


