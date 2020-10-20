project('call_that', ['cpp', 'c'],
  version: '0.0.97',
  license: 'MIT',
  default_options: [
    'cpp_std=c++17',
    'c_std=c17',
# For standalone project build tests and examples by default
    'build_examples=@0@'.format(not meson.is_subproject()),
    # TODO: 'build_tests=@0@'.format(not meson.is_subproject()),
  ]
)

cxx = meson.get_compiler('cpp')

# Ensure c++ standard version is at very least 201703L (c++17)
_cpp_std_ver = cxx.get_define('__cplusplus')
assert(_cpp_std_ver.endswith('L'))
_cpp_std_ver = _cpp_std_ver.substring(0, -1).to_int() # remove 'L'

if not (_cpp_std_ver >= 201703)
  error('Requires c++17 or newer (__cplusplus >= 201703L)')
endif

# TODO: Ensure that last released boost version is sufficient 
hana_dep = dependency('boost', modules: ['hana'], version: '>=1.74.0', required: false)
hana_as_subproject = true # not hana_dep.found()

if hana_as_subproject
  hana_dep = dependency('boost', modules: ['hana'], version: '>=1.7.0',
                         fallback: ['hana-git', 'hana_dep'], required: true)
  add_project_arguments('-DCT_HANA_EMBEDDED=1', language: ['cpp'])
endif

inc = include_directories('include')

if get_option('build_examples')
   glfw_dep = dependency('glfw', version: '>=3.2.2', required: false)

   if not glfw_dep.found()
      cmake = get_variable('cmake', import('cmake'))
      glfw_opts = cmake.subproject_options()
      glfw_opts.add_cmake_defines({
        'GLFW_BUILD_DOCS': false,
        'GLFW_BUILD_EXAMPLES' : false,
      })
      glfw_proj = cmake.subproject('glfw-git', options: glfw_opts)
      glfw_cm_dep = glfw_proj.dependency('glfw')
      meson.override_dependency('glfw', glfw_cm_dep)
      # meson builds PIC, thus glfw didn't figure it out that it needs libdl
      glfw_dep = declare_dependency(
        dependencies: [dependency('glfw', required: true), cxx.find_library('dl')]
      )
   endif
endif

if get_option('build_tests')
   cmake = get_variable('cmake', import('cmake'))
   opts = cmake.subproject_options()
   catch2_proj = cmake.subproject('catch2-git', options: opts)
   message('CMaket targets:\n - ' + '\n - '.join(catch2_proj.target_list()))
   catch2_cm_dep = catch2_proj.dependency('Catch2')
   meson.override_dependency('catch2', catch2_cm_dep)
   catch2_dep = dependency('catch2', required: true)
endif

call_that_dep = declare_dependency(include_directories: inc, dependencies: [hana_dep])

subdir('examples')
subdir('tests')
