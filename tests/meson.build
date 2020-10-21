if not get_option('build_tests')
  subdir_done()
endif

assert(catch2_dep.found())

_ct_srcs = ['unit_runner.cc']

_ct_test = executable('ct_tests', _ct_srcs, dependencies: [catch2_dep, call_that_dep])
