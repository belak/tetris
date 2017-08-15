project('tetris', 'cpp', default_options: ['cpp_std=c++11'])
allegro_deps = [
    'allegro-5',
    'allegro_main-5',
    'allegro_ttf-5',
    'allegro_font-5',
    'allegro_primitives-5',
    'allegro_audio-5',
    'allegro_acodec-5',
    'allegro_color-5',
    'allegro_image-5'
]

bin_deps = []
foreach adep : allegro_deps
    bin_deps += dependency(adep)
endforeach

bin_src = [
    'src/director.cpp',
    'src/main.cpp',
    'src/screens/background.cpp',
    'src/screens/game.cpp',
    'src/screens/menu.cpp',
    'src/screens/splash.cpp',
    'src/tetrominos.cpp',
    'src/utils.cpp'
]

bin_exe = executable('tetris', bin_src, dependencies : bin_deps, cpp_args : ['-Wall'])
run_target('run', depends : bin_exe, command : [bin_exe.full_path(), meson.source_root()])
