<?xml version="1.0"?>
<interface>
  <!-- interface-requires gtk+ 2.8 -->
  <!-- interface-naming-policy project-wide -->
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
              <object class="GtkLabel" id="primary_lbl">
                <property name="visible">True</property>
                <property name="xalign">0</property>
                <property name="label" translatable="yes">&lt;span weight="bold" size="larger"&gt;Rebuilding gamelist&lt;/span	&gt;</property>
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
                      <object class="GtkLabel" id="secondary_lbl">
                        <property name="visible">True</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">GMAMEUI is getting information about the
ROMs supported by this version of MAME.</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">False</property>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkProgressBar" id="pgrsbar_listoutput">
                        <property name="visible">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">False</property>
                        <property name="position">1</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkLabel" id="action_lbl">
                        <property name="visible">True</property>
                        <property name="xalign">0</property>
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
                <property name="expand">False</property>
                <property name="fill">False</property>
                <property name="position">1</property>
              </packing>
            </child>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="fill">False</property>
            <property name="padding">6</property>
            <property name="position">1</property>
          </packing>
        </child>
        <child internal-child="action_area">
          <object class="GtkHButtonBox" id="dialog-action_area1">
            <property name="visible">True</property>
            <property name="layout_style">end</property>
            <child>
              <object class="GtkButton" id="btn_cancel">
                <property name="label" translatable="yes">gtk-cancel</property>
                <property name="visible">True</property>
                <property name="can_focus">True</property>
                <property name="receives_default">True</property>
                <property name="use_stock">True</property>
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
            <property name="pack_type">end</property>
            <property name="position">0</property>
          </packing>
        </child>
      </object>
    </child>
    <action-widgets>
      <action-widget response="0">btn_cancel</action-widget>
    </action-widgets>
  </object>
</interface>
