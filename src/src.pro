TEMPLATE = subdirs

# this order is important
unset(SRC_SUBDIRS)
win32:SRC_SUBDIRS += src_winmain
SRC_SUBDIRS += src_corelib src_xml src_network src_sql src_testlib
contains(QT_CONFIG, dbus):SRC_SUBDIRS += src_dbus
!contains(QT_CONFIG, no-gui): SRC_SUBDIRS += src_gui

include(tools/tools.pro)

contains(QT_CONFIG, opengl)|contains(QT_CONFIG, opengles1)|contains(QT_CONFIG, opengles2): SRC_SUBDIRS += src_opengl
contains(QT_CONFIG, xmlpatterns): SRC_SUBDIRS += src_xmlpatterns
contains(QT_CONFIG, svg): SRC_SUBDIRS += src_svg
contains(QT_CONFIG, script): SRC_SUBDIRS += src_script
contains(QT_CONFIG, declarative): SRC_SUBDIRS += src_declarative
!contains(QT_CONFIG, no-gui):contains(QT_CONFIG, scripttools): SRC_SUBDIRS += src_scripttools
SRC_SUBDIRS += src_plugins
contains(QT_CONFIG, declarative): SRC_SUBDIRS += src_imports

src_winmain.subdir = $$QT_SOURCE_TREE/src/winmain
src_winmain.target = sub-winmain
src_corelib.subdir = $$QT_SOURCE_TREE/src/corelib
src_corelib.target = sub-corelib
src_xml.subdir = $$QT_SOURCE_TREE/src/xml
src_xml.target = sub-xml
src_xmlpatterns.subdir = $$QT_SOURCE_TREE/src/xmlpatterns
src_xmlpatterns.target = sub-xmlpatterns
src_dbus.subdir = $$QT_SOURCE_TREE/src/dbus
src_dbus.target = sub-dbus
src_gui.subdir = $$QT_SOURCE_TREE/src/gui
src_gui.target = sub-gui
src_sql.subdir = $$QT_SOURCE_TREE/src/sql
src_sql.target = sub-sql
src_network.subdir = $$QT_SOURCE_TREE/src/network
src_network.target = sub-network
src_svg.subdir = $$QT_SOURCE_TREE/src/svg
src_svg.target = sub-svg
src_script.subdir = $$QT_SOURCE_TREE/src/script
src_script.target = sub-script
src_scripttools.subdir = $$QT_SOURCE_TREE/src/scripttools
src_scripttools.target = sub-scripttools
src_opengl.subdir = $$QT_SOURCE_TREE/src/opengl
src_opengl.target = sub-opengl
src_plugins.subdir = $$QT_SOURCE_TREE/src/plugins
src_plugins.target = sub-plugins
src_imports.subdir = $$QT_SOURCE_TREE/src/imports
src_imports.target = sub-imports
src_testlib.subdir = $$QT_SOURCE_TREE/src/testlib
src_testlib.target = sub-testlib
src_declarative.subdir = $$QT_SOURCE_TREE/src/declarative
src_declarative.target = sub-declarative

#CONFIG += ordered
!ordered {
   src_corelib.depends = src_tools_moc src_tools_rcc
   src_gui.depends = src_corelib src_tools_uic
   embedded: src_gui.depends += src_network
   src_xml.depends = src_corelib
   src_xmlpatterns.depends = src_corelib src_network
   src_dbus.depends = src_corelib
   src_svg.depends = src_corelib src_gui
   src_script.depends = src_corelib
   src_scripttools.depends = src_script src_gui src_network
   src_network.depends = src_corelib
   src_opengl.depends = src_gui
   src_sql.depends = src_corelib
   src_testlib.depends = src_corelib
   src_tools_idc.depends = src_corelib             # target defined in tools.pro
   src_declarative.depends = src_gui src_script src_network
   src_plugins.depends = src_gui src_sql src_svg
   contains(QT_CONFIG, declarative):src_plugins.depends += src_declarative
   src_imports.depends = src_gui src_declarative
   contains(QT_CONFIG, dbus):{
      src_plugins.depends += src_dbus
   }
   contains(QT_CONFIG, opengl)|contains(QT_CONFIG, opengles1)|contains(QT_CONFIG, opengles2) {
      src_plugins.depends += src_opengl
      src_declarative.depends += src_opengl
   }
   contains(QT_CONFIG, xmlpatterns) {
      src_declarative.depends += src_xmlpatterns
   }
   contains(QT_CONFIG, svg) {
      src_declarative.depends += src_svg
   }
}


# This creates a sub-src rule
sub_src_target.CONFIG = recursive
sub_src_target.recurse = $$TOOLS_SUBDIRS $$SRC_SUBDIRS
sub_src_target.target = sub-src
sub_src_target.recurse_target =
QMAKE_EXTRA_TARGETS += sub_src_target

# This gives us a top level debug/release
for(subname, SRC_SUBDIRS) {
   subdir = $$subname
   !isEmpty($${subname}.subdir):subdir = $$eval($${subname}.subdir)
   subpro = $$subdir/$${basename(subdir)}.pro
   !exists($$subpro):next()
   subtarget = $$replace(subdir, [^A-Za-z0-9], _)
   reg_src = $$replace(QT_SOURCE_TREE, \\\\, \\\\)
   subdir = $$replace(subdir, $$reg_src, $$QT_BUILD_TREE)
   subdir = $$replace(subdir, /, $$QMAKE_DIR_SEP)
   subdir = $$replace(subdir, \\\\, $$QMAKE_DIR_SEP)
   SUB_TEMPLATE = $$list($$fromfile($$subpro, TEMPLATE))
   !isEqual(subname, src_tools_bootstrap):if(isEqual($$SUB_TEMPLATE, lib) | isEqual($$SUB_TEMPLATE, subdirs) | isEqual(subname, src_tools_idc) | isEqual(subname, src_tools_uic3)):!separate_debug_info {
       #debug
       debug-$${subtarget}.depends = $${subdir}$${QMAKE_DIR_SEP}$(MAKEFILE) $$EXTRA_DEBUG_TARGETS
       debug-$${subtarget}.commands = (cd $$subdir && $(MAKE) -f $(MAKEFILE) debug)
       EXTRA_DEBUG_TARGETS += debug-$${subtarget}
       QMAKE_EXTRA_TARGETS += debug-$${subtarget}
       #release
       release-$${subtarget}.depends = $${subdir}$${QMAKE_DIR_SEP}$(MAKEFILE) $$EXTRA_RELEASE_TARGETS
       release-$${subtarget}.commands = (cd $$subdir && $(MAKE) -f $(MAKEFILE) release)
       EXTRA_RELEASE_TARGETS += release-$${subtarget}
       QMAKE_EXTRA_TARGETS += release-$${subtarget}
    } else { #do not have a real debug target/release
       #debug
       debug-$${subtarget}.depends = $${subdir}$${QMAKE_DIR_SEP}$(MAKEFILE) $$EXTRA_DEBUG_TARGETS
       debug-$${subtarget}.commands = (cd $$subdir && $(MAKE) -f $(MAKEFILE) first)
       EXTRA_DEBUG_TARGETS += debug-$${subtarget}
       QMAKE_EXTRA_TARGETS += debug-$${subtarget}
       #release
       release-$${subtarget}.depends = $${subdir}$${QMAKE_DIR_SEP}$(MAKEFILE) $$EXTRA_RELEASE_TARGETS
       release-$${subtarget}.commands = (cd $$subdir && $(MAKE) -f $(MAKEFILE) first)
       EXTRA_RELEASE_TARGETS += release-$${subtarget}
       QMAKE_EXTRA_TARGETS += release-$${subtarget}
   }
}
debug.depends = $$EXTRA_DEBUG_TARGETS
release.depends = $$EXTRA_RELEASE_TARGETS
QMAKE_EXTRA_TARGETS += debug release

SUBDIRS += $$SRC_SUBDIRS
