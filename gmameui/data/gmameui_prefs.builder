<?xml version="1.0"?>
<interface>
  <requires lib="gtk+" version="2.16"/>
  <!-- interface-naming-policy toplevel-contextual -->
  <object class="GtkDialog" id="dialog1">
    <property name="border_width">5</property>
    <property name="title" translatable="yes">GMAMEUI Preferences</property>
    <property name="window_position">center-on-parent</property>
    <property name="type_hint">dialog</property>
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
            <child>
              <object class="GtkVBox" id="vbox2">
                <property name="visible">True</property>
                <property name="orientation">vertical</property>
                <property name="spacing">6</property>
                <child>
                  <object class="GtkLabel" id="label1">
                    <property name="visible">True</property>
                    <property name="xalign">0</property>
                    <property name="label" translatable="yes">&lt;b&gt;Startup Options&lt;/b&gt;</property>
                    <property name="use_markup">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">False</property>
                    <property name="padding">6</property>
                    <property name="position">0</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkAlignment" id="alignment1">
                    <property name="visible">True</property>
                    <property name="left_padding">12</property>
                    <child>
                      <object class="GtkVBox" id="vbox3">
                        <property name="visible">True</property>
                        <property name="orientation">vertical</property>
                        <property name="spacing">6</property>
                        <child>
                          <object class="GtkCheckButton" id="gamecheck">
                            <property name="label" translatable="yes">Search for new games</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">False</property>
                            <property name="draw_indicator">True</property>
                          </object>
                          <packing>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkCheckButton" id="versioncheck">
                            <property name="label" translatable="yes">Enable version mismatch warning</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">False</property>
                            <property name="draw_indicator">True</property>
                          </object>
                          <packing>
                            <property name="position">1</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkCheckButton" id="usexmameoptions">
                            <property name="label" translatable="yes">Use MAME default options</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">False</property>
                            <property name="draw_indicator">True</property>
                          </object>
                          <packing>
                            <property name="position">2</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkCheckButton" id="usejoyingui">
                            <property name="label" translatable="yes">Allow game selection with a joystick</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">False</property>
                            <property name="draw_indicator">True</property>
                          </object>
                          <packing>
                            <property name="position">3</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkHBox" id="hbox1">
                            <property name="visible">True</property>
                            <property name="spacing">6</property>
                            <child>
                              <object class="GtkLabel" id="gui_joy_label">
                                <property name="visible">True</property>
                                <property name="xalign">0</property>
                                <property name="label" translatable="yes">Joystick device:</property>
                              </object>
                              <packing>
                                <property name="expand">False</property>
                                <property name="fill">False</property>
                                <property name="position">0</property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkEntry" id="gui_joy_entry">
                                <property name="visible">True</property>
                                <property name="can_focus">True</property>
                                <property name="max_length">20</property>
                                <property name="invisible_char">&#x25CF;</property>
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
                            <property name="position">4</property>
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
                <property name="padding">6</property>
                <property name="position">0</property>
              </packing>
            </child>
            <child>
              <object class="GtkVBox" id="vbox4">
                <property name="visible">True</property>
                <property name="orientation">vertical</property>
                <property name="spacing">6</property>
                <child>
                  <object class="GtkLabel" id="label4">
                    <property name="visible">True</property>
                    <property name="xalign">0</property>
                    <property name="label" translatable="yes">&lt;b&gt;User Interface Options&lt;/b&gt;</property>
                    <property name="use_markup">True</property>
                  </object>
                  <packing>
                    <property name="position">0</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkAlignment" id="alignment2">
                    <property name="visible">True</property>
                    <property name="left_padding">12</property>
                    <child>
                      <object class="GtkVBox" id="vbox5">
                        <property name="visible">True</property>
                        <property name="orientation">vertical</property>
                        <property name="spacing">6</property>
                        <child>
                          <object class="GtkCheckButton" id="theprefix">
                            <property name="label" translatable="yes">'The' as a prefix</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">False</property>
                            <property name="draw_indicator">True</property>
                          </object>
                          <packing>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkCheckButton" id="prefercustomicons">
                            <property name="label" translatable="yes">Prefer custom icons over status icons</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">False</property>
                            <property name="draw_indicator">True</property>
                          </object>
                          <packing>
                            <property name="position">1</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="label2">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Visible columns</property>
                            <property name="use_markup">True</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="fill">False</property>
                            <property name="position">2</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkAlignment" id="alignment3">
                            <property name="visible">True</property>
                            <property name="left_padding">12</property>
                            <child>
                              <object class="GtkTable" id="table1">
                                <property name="visible">True</property>
                                <property name="n_rows">5</property>
                                <property name="n_columns">2</property>
                                <property name="column_spacing">6</property>
                                <property name="row_spacing">6</property>
                                <child>
                                  <object class="GtkCheckButton" id="col_samples">
                                    <property name="label" translatable="yes">Samples</property>
                                    <property name="visible">True</property>
                                    <property name="can_focus">True</property>
                                    <property name="receives_default">False</property>
                                    <property name="draw_indicator">True</property>
                                  </object>
                                  <packing>
                                    <property name="top_attach">1</property>
                                    <property name="bottom_attach">2</property>
                                    <property name="x_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkCheckButton" id="col_directory">
                                    <property name="label" translatable="yes">Directory</property>
                                    <property name="visible">True</property>
                                    <property name="can_focus">True</property>
                                    <property name="receives_default">False</property>
                                    <property name="draw_indicator">True</property>
                                  </object>
                                  <packing>
                                    <property name="top_attach">2</property>
                                    <property name="bottom_attach">3</property>
                                    <property name="x_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkCheckButton" id="col_playcount">
                                    <property name="label" translatable="yes">Playcount</property>
                                    <property name="visible">True</property>
                                    <property name="can_focus">True</property>
                                    <property name="receives_default">False</property>
                                    <property name="draw_indicator">True</property>
                                  </object>
                                  <packing>
                                    <property name="top_attach">3</property>
                                    <property name="bottom_attach">4</property>
                                    <property name="x_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkCheckButton" id="col_manufacturer">
                                    <property name="label" translatable="yes">Manufacturer</property>
                                    <property name="visible">True</property>
                                    <property name="can_focus">True</property>
                                    <property name="receives_default">False</property>
                                    <property name="draw_indicator">True</property>
                                  </object>
                                  <packing>
                                    <property name="top_attach">4</property>
                                    <property name="bottom_attach">5</property>
                                    <property name="x_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkCheckButton" id="col_year">
                                    <property name="label" translatable="yes">Year</property>
                                    <property name="visible">True</property>
                                    <property name="can_focus">True</property>
                                    <property name="receives_default">False</property>
                                    <property name="draw_indicator">True</property>
                                  </object>
                                  <packing>
                                    <property name="left_attach">1</property>
                                    <property name="right_attach">2</property>
                                    <property name="x_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkCheckButton" id="col_cloneof">
                                    <property name="label" translatable="yes">Clone of</property>
                                    <property name="visible">True</property>
                                    <property name="can_focus">True</property>
                                    <property name="receives_default">False</property>
                                    <property name="draw_indicator">True</property>
                                  </object>
                                  <packing>
                                    <property name="left_attach">1</property>
                                    <property name="right_attach">2</property>
                                    <property name="top_attach">1</property>
                                    <property name="bottom_attach">2</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkCheckButton" id="col_driver">
                                    <property name="label" translatable="yes">Driver</property>
                                    <property name="visible">True</property>
                                    <property name="can_focus">True</property>
                                    <property name="receives_default">False</property>
                                    <property name="draw_indicator">True</property>
                                  </object>
                                  <packing>
                                    <property name="left_attach">1</property>
                                    <property name="right_attach">2</property>
                                    <property name="top_attach">2</property>
                                    <property name="bottom_attach">3</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkCheckButton" id="col_version">
                                    <property name="label" translatable="yes">Version</property>
                                    <property name="visible">True</property>
                                    <property name="can_focus">True</property>
                                    <property name="receives_default">False</property>
                                    <property name="draw_indicator">True</property>
                                  </object>
                                  <packing>
                                    <property name="left_attach">1</property>
                                    <property name="right_attach">2</property>
                                    <property name="top_attach">3</property>
                                    <property name="bottom_attach">4</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkCheckButton" id="col_category">
                                    <property name="label" translatable="yes">Category</property>
                                    <property name="visible">True</property>
                                    <property name="can_focus">True</property>
                                    <property name="receives_default">False</property>
                                    <property name="draw_indicator">True</property>
                                  </object>
                                  <packing>
                                    <property name="left_attach">1</property>
                                    <property name="right_attach">2</property>
                                    <property name="top_attach">4</property>
                                    <property name="bottom_attach">5</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkCheckButton" id="col_gamename">
                                    <property name="label" translatable="yes">Game Name</property>
                                    <property name="visible">True</property>
                                    <property name="sensitive">False</property>
                                    <property name="can_focus">True</property>
                                    <property name="receives_default">False</property>
                                    <property name="draw_indicator">True</property>
                                  </object>
                                  <packing>
                                    <property name="x_options">GTK_FILL</property>
                                  </packing>
                                </child>
                              </object>
                            </child>
                          </object>
                          <packing>
                            <property name="position">3</property>
                          </packing>
                        </child>
                      </object>
                    </child>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">False</property>
                    <property name="padding">6</property>
                    <property name="position">1</property>
                  </packing>
                </child>
              </object>
              <packing>
                <property name="padding">6</property>
                <property name="position">1</property>
              </packing>
            </child>
          </object>
          <packing>
            <property name="position">1</property>
          </packing>
        </child>
        <child internal-child="action_area">
          <object class="GtkHButtonBox" id="dialog-action_area1">
            <property name="visible">True</property>
            <property name="layout_style">end</property>
            <child>
              <object class="GtkButton" id="btn_close">
                <property name="label">gtk-close</property>
                <property name="visible">True</property>
                <property name="can_focus">True</property>
                <property name="can_default">True</property>
                <property name="has_default">True</property>
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
      <action-widget response="-7">btn_close</action-widget>
    </action-widgets>
  </object>
</interface>
