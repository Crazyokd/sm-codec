sources = [
    'encoder.c',
    'decoder.c',
]

lol_dep = dependency('lol', fallback : ['lol', 'lol_dep'])

sm_codec = library(
    'sm-codec',
    sources,
    include_directories: [comm_inc],
    dependencies: [lol_dep],
    install_dir: libdir,
    install: true,
)

sm_codec_dep = declare_dependency(
    link_with : sm_codec,
    include_directories: [comm_inc],
    dependencies: [lol_dep],
)