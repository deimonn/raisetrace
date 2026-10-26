project(
    'raisetrace', 'c',
    version: '0.2.0',
    license: 'MIT',
    license_files: 'LICENSE',
    meson_version: '>=1.1',
    default_options: [
        'c_std=c99',
        'warning_level=3'
    ]
)

backtrace = declare_dependency(link_args: ['-lbacktrace'])
md2manc = find_program('md2manc', required: get_option('man-pages'))

add_project_arguments(
    '-D_POSIX_C_SOURCE=200809L',
    '-Wvla',
    '-Wno-empty-translation-unit',
    language: 'c'
)

headers = files('include/raisetrace.h')

if host_machine.system() == 'windows'
    sources = files('src/raisetrace-win32.c')
else
    sources = files('src/raisetrace-unix.c')
endif

install_headers(headers)

raisetrace = library(
    'raisetrace',
    sources,
    dependencies: [backtrace],
    include_directories: 'include',
    install: true
)

pkgconfig = import('pkgconfig')
pkgconfig.generate(
    raisetrace,
    name: 'raisetrace',
    description: 'C library for generating stack traces on signal/exception',
    url: 'https://github.com/deimonn/raisetrace'
)

if md2manc.found()
    date = run_command('date', '+%Y-%m-%d', check: true).stdout().strip()
    docs = [
        'raisetrace.3',
        'raisetrace_callback.3',
        'raisetrace_faults.3'
    ]

    foreach doc : docs
        custom_target(
            doc,
            command: [
                md2manc,
                '-d', date,
                '-S', 'raisetrace ' + meson.project_version(),
                '@INPUT@'
            ],
            input: 'docs/' + doc + '.md',
            output: doc,
            capture: true,
            install: true,
            install_dir: get_option('mandir') + '/man3'
        )
    endforeach

    install_man(
        'docs/RAISETRACE_CONTINUE.3',
        'docs/RAISETRACE_DEFAULT.3',
        'docs/raisetrace_unhandled.3'
    )
endif

meson.add_postconf_script('compile_flags.py', 'raisetrace')
