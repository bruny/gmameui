<?xml version="1.0"?>
<interface>
  <requires lib="gtk+" version="2.16"/>
  <!-- interface-naming-policy project-wide -->
  <object class="GtkListStore" id="liststore1">
    <columns>
      <!-- column-name Rom -->
      <column type="gchar"/>
      <!-- column-name RomStatus -->
      <column type="gchar"/>
      <!-- column-name CRC -->
      <column type="gchar"/>
    </columns>
  </object>
  <object class="GtkTreeStore" id="treestore1">
    <columns>
      <!-- column-name Romset -->
      <column type="gchararray"/>
      <!-- column-name Region -->
      <column type="gchararray"/>
      <!-- column-name Status -->
      <column type="gchararray"/>
      <!-- column-name OK -->
      <column type="gint"/>
    </columns>
  </object>
  <object class="GtkDialog" id="dialog1">
    <property name="border_width">5</property>
    <property name="type_hint">normal</property>
    <property name="has_separator">False</property>
    <child internal-child="vbox">
      <object class="GtkVBox" id="dialog-vbox1">
        <property name="visible">True</property>
        <property name="orientation">vertical</property>
        <property name="spacing">2</property>
        <child>
          <object class="GtkVBox" id="vbox1">
            <property name="visible">True</property>
            <property name="orientation">vertical</property>
            <property name="spacing">6</property>
            <child>
              <object class="GtkLabel" id="label1">
                <property name="visible">True</property>
                <property name="xalign">0</property>
                <property name="label" translatable="yes">&lt;b&gt;Romset Status&lt;/b&gt;</property>
                <property name="use_markup">True</property>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="fill">False</property>
                <property name="position">0</property>
              </packing>
            </child>
            <child>
              <object class="GtkAlignment" id="alignment1">
                <property name="visible">True</property>
                <property name="left_padding">12</property>
                <child>
                  <object class="GtkVBox" id="vbox2">
                    <property name="visible">True</property>
                    <property name="orientation">vertical</property>
                    <property name="spacing">6</property>
                    <child>
                      <object class="GtkScrolledWindow" id="scrolledwindow1">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="hscrollbar_policy">automatic</property>
                        <property name="vscrollbar_policy">automatic</property>
                        <property name="shadow_type">in</property>
                        <child>
                          <object class="GtkTreeView" id="romset_treeview">
                            <property name="width_request">600</property>
                            <property name="height_request">400</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="model">treestore1</property>
                            <property name="reorderable">True</property>
                            <property name="search_column">0</property>
                            <property name="enable_tree_lines">True</property>
                            <child>
                              <object class="GtkTreeViewColumn" id="RomsetCol">
                                <property name="resizable">True</property>
                                <property name="sizing">autosize</property>
                                <property name="title">Romset</property>
                                <property name="expand">True</property>
                                <property name="clickable">True</property>
                                <property name="sort_indicator">True</property>
                                <child>
                                  <object class="GtkCellRendererText" id="RomsetName"/>
                                  <attributes>
                                    <attribute name="text">0</attribute>
                                  </attributes>
                                </child>
                              </object>
                            </child>
                            <child>
                              <object class="GtkTreeViewColumn" id="RomRegionCol">
                                <property name="resizable">True</property>
                                <property name="sizing">autosize</property>
                                <property name="title">Region</property>
                                <child>
                                  <object class="GtkCellRendererText" id="RomRegion"/>
                                  <attributes>
                                    <attribute name="text">1</attribute>
                                  </attributes>
                                </child>
                              </object>
                            </child>
                            <child>
                              <object class="GtkTreeViewColumn" id="RomsetStatusCol">
                                <property name="resizable">True</property>
                                <property name="sizing">autosize</property>
                                <property name="title">Status</property>
                                <property name="expand">True</property>
                                <property name="clickable">True</property>
                                <property name="sort_indicator">True</property>
                                <child>
                                  <object class="GtkCellRendererText" id="RomsetStatus"/>
                                  <attributes>
                                    <attribute name="text">2</attribute>
                                  </attributes>
                                </child>
                              </object>
                            </child>
                            <child>
                              <object class="GtkTreeViewColumn" id="OKCol">
                                <property name="visible">False</property>
                                <property name="resizable">True</property>
                                <property name="sizing">autosize</property>
                                <property name="title">OK</property>
                              </object>
                            </child>
                          </object>
                        </child>
                      </object>
                      <packing>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="chk_hideok">
                        <property name="label" translatable="yes">Hide correct romsets</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="xalign">0</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">False</property>
                        <property name="position">1</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkHButtonBox" id="hbuttonbox1">
                        <property name="visible">True</property>
                        <property name="layout_style">start</property>
                        <child>
                          <object class="GtkButton" id="btn_fix">
                            <property name="label" translatable="yes">button</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="fill">False</property>
                            <property name="position">0</property>
                          </packing>
                        </child>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">False</property>
                        <property name="position">2</property>
                      </packing>
                    </child>
                  </object>
                </child>
              </object>
              <packing>
                <property name="position">1</property>
              </packing>
            </child>
          </object>
          <packing>
            <property name="padding">6</property>
            <property name="position">1</property>
          </packing>
        </child>
        <child internal-child="action_area">
          <object class="GtkHButtonBox" id="dialog-action_area1">
            <property name="visible">True</property>
            <property name="layout_style">end</property>
            <child>
              <placeholder/>
            </child>
            <child>
              <placeholder/>
            </child>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="pack_type">end</property>
            <property name="position">0</property>
          </packing>
        </child>
      </object>
    </child>
  </object>
</interface>
