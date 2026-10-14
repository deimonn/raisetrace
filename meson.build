project(
    'sigtrace', 'c',
    version: '0.2.0',
    license: 'MIT',
    license_files: 'LICENSE',
    meson_version: '>=1.1',
    default_options: [
        'c_std=c99',
        'warning_level=3'
    ]
)

compiler = meson.get_compiler('c')

backtrace = compiler.find_library('backtrace')
md2manc = find_program('md2manc', required: get_option('man-pages'))

add_project_arguments(
    '-D_POSIX_C_SOURCE=200809L',
    '-Wvla',
    '-Wno-empty-translation-unit',
    language: 'c'
)

headers = files('include/sigtrace.h')
sources = files('src/sigtrace.c')

install_headers(headers)

sigtrace = static_library( # sigtrace as a shared library fails to link
    'sigtrace',
    sources,
    dependencies: [backtrace],
    include_directories: 'include',
    install: true
)

pkgconfig = import('pkgconfig')
pkgconfig.generate(
    sigtrace,
    name: 'sigtrace',
    description: 'C library for generating stack traces on fault/signal',
    url: 'https://github.com/deimonn/sigtrace'
)

if md2manc.found()
    date = run_command('date', '+%Y-%m-%d', check: true).stdout().strip()

    custom_target(
        'sigtrace.3',
        command: [
            md2manc,
            '-d', date,
            '-S', 'sigtrace ' + meson.project_version(),
            '@INPUT@'
        ],
        input: 'docs/sigtrace.3.md',
        output: 'sigtrace.3',
        capture: true,
        install: true,
        install_dir: get_option('mandir') + '/man3'
    )

    install_man(
        'docs/sigtrace_faults.3',
        'docs/sigtrace_unhandled.3'
    )
endif

meson.add_postconf_script('compile_flags.py', 'sigtrace')
