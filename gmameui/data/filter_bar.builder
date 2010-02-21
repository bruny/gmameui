<?xml version="1.0"?>
<interface>
  <requires lib="gtk+" version="2.16"/>
  <!-- interface-naming-policy toplevel-contextual -->
  <object class="GtkWindow" id="window1">
    <child>
      <object class="GtkHBox" id="filter_hbox">
        <property name="visible">True</property>
        <property name="spacing">6</property>
        <child>
          <placeholder/>
        </child>
        <child>
          <object class="GtkRadioButton" id="filter_btn_unavail">
            <property name="label" translatable="yes">Unavailable ROMs</property>
            <property name="visible">True</property>
            <property name="can_focus">True</property>
            <property name="receives_default">False</property>
            <property name="draw_indicator">True</property>
            <property name="group">filter_btn_all</property>
          </object>
          <packing>
            <property name="pack_type">end</property>
            <property name="position">4</property>
          </packing>
        </child>
        <child>
          <object class="GtkRadioButton" id="filter_btn_avail">
            <property name="label" translatable="yes">Available ROMs</property>
            <property name="visible">True</property>
            <property name="can_focus">True</property>
            <property name="receives_default">False</property>
            <property name="draw_indicator">True</property>
            <property name="group">filter_btn_all</property>
          </object>
          <packing>
            <property name="pack_type">end</property>
            <property name="position">3</property>
          </packing>
        </child>
        <child>
          <object class="GtkRadioButton" id="filter_btn_all">
            <property name="label" translatable="yes">All ROMs</property>
            <property name="visible">True</property>
            <property name="can_focus">True</property>
            <property name="receives_default">False</property>
            <property name="active">True</property>
            <property name="draw_indicator">True</property>
          </object>
          <packing>
            <property name="pack_type">end</property>
            <property name="position">2</property>
          </packing>
        </child>
      </object>
    </child>
  </object>
</interface>
