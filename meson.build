project('evol-headers', 'c',
  default_options : [ 'c_std=gnu23' ])

headers_include = include_directories('.')

evh_c_args = []
buildtype = get_option('buildtype')
if buildtype == 'debug'
  evh_c_args += '-DEV_BUILDTYPE_DEBUG=1'
elif buildtype == 'debugoptimized'
  evh_c_args += '-DEV_BUILDTYPE_DEBUGOPT=1'
else
  evh_c_args += '-DEV_BUILDTYPE_RELEASE=1'
endif

cc = meson.get_compiler('c')
if cc.get_id() == 'msvc'
  evh_c_args += '/Zc:preprocessor'
elif cc.get_id() == 'clang'
  evh_c_args += '-DEV_CC_CLANG=1'
endif

# All other targets should follow the same template
str_lib = static_library('ev_str', files('buildfiles/ev_str.c'), c_args: evh_c_args)
vec_lib = static_library('ev_vec', files('buildfiles/ev_vec.c'), c_args: evh_c_args)
helpers_lib = static_library('ev_helpers', files('buildfiles/ev_helpers.c'), c_args: evh_c_args)

str_dep = declare_dependency(link_with: str_lib, include_directories: headers_include)
vec_dep = declare_dependency(link_with: vec_lib, include_directories: headers_include)
helpers_dep = declare_dependency(link_with: helpers_lib, include_directories: headers_include)

headers_dep = declare_dependency(
  dependencies: [
    str_dep,
    vec_dep,
    helpers_dep,
  ]
)

# Tests
str_test = executable('str_test', 'str_test.c', dependencies: [str_dep], c_args: evh_c_args)
test('evstr', str_test)

if meson.version().version_compare('>= 0.54.0')
  meson.override_dependency('ev_vec', vec_dep)
  meson.override_dependency('ev_str', str_dep)
  meson.override_dependency('ev_helpers', helpers_dep)
  meson.override_dependency('evol-headers', headers_dep)
endif
