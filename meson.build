project('iwqt', 'cpp', default_options:[
    'buildtype=debugoptimized'
])

qt6 = import('qt6')

qt6_dep = dependency('qt6', modules: ['Core', 'Gui', 'Widgets'])

dbus_dep = dependency('sdbus-c++')

sources = [
  'main.cpp',
  'tray.cpp',
  'known_window.cpp',
  'manage_window.cpp',
  'iwd.cpp',
  'device.cpp',
  'agent.cpp',
  'network.cpp'
]

#lang_cpp = qt6.compile_translations(qresource: 'lang.qrc')

preprocessed = qt6.preprocess(
  moc_headers: ['tray.hpp', 'known_window.hpp', 'manage_window.hpp'], 
  qresources: ['systray.qrc']
)

executable('iwqt', sources + preprocessed, dependencies: [qt6_dep, dbus_dep], install: true, install_dir: get_option('bindir'))

install_data('images/good.png', rename: 'icon.png', install_dir: get_option('datadir') / 'iwqt')
install_data('iwqt.desktop', install_dir: get_option('datadir') / 'applications')
