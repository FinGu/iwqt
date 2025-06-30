project('iwqt', 'cpp', default_options:[
    'buildtype=debugoptimized'
])

qt6 = import('qt6')

qt6_dep = dependency('qt6', modules: ['Core', 'Gui', 'Widgets'])

dbus_dep = dependency('sdbus-c++')

sources = [
  'main.cpp',
  'window.cpp',
  'iwd.cpp',
  'device.cpp',
  'agent.cpp',
  'network.cpp'
]

#lang_cpp = qt6.compile_translations(qresource: 'lang.qrc')

preprocessed = qt6.preprocess(
  moc_headers: ['window.hpp'], 
  qresources: ['systray.qrc']
)

executable('out', sources + preprocessed, dependencies: [qt6_dep, dbus_dep])
