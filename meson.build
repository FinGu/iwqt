project('iwqt', 'cpp', default_options:[])

qt6 = import('qt6')

qt6_dep = dependency('qt6', modules: ['Core', 'Gui', 'Widgets', 'Svg'])

dbus_dep = dependency('sdbus-c++')

sources = [
  'main.cpp',
  'tray.cpp',
  'add_edit_window.cpp',
  'manage_window.cpp',
  'iwd.cpp',
  'adapter.cpp',
  'device.cpp',
  'agent.cpp',
  'network.cpp',
  'file.cpp',
  'utils.cpp'
]

#lang_cpp = qt6.compile_translations(qresource: 'lang.qrc')

preprocessed = qt6.preprocess(
  moc_headers: ['tray.hpp', 'add_edit_window.hpp', 'manage_window.hpp'], 
  qresources: ['systray.qrc']
)

executable('iwqt', sources + preprocessed, dependencies: [qt6_dep, dbus_dep], install: true, install_dir: get_option('bindir'))

install_data('images/wireless-4.svg', rename: 'icon.svg', install_dir: get_option('datadir') / 'iwqt')
install_data('iwqt.desktop', install_dir: get_option('datadir') / 'applications')
