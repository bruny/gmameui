<?xml version="1.0"?>
<interface>
  <!-- interface-requires gtk+ 2.6 -->
  <!-- interface-naming-policy toplevel-contextual -->
  <object class="GtkDialog" id="dialog1">
    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
    <property name="border_width">5</property>
    <property name="window_position">center-on-parent</property>
    <property name="type_hint">dialog</property>
    <property name="has_separator">False</property>
    <child internal-child="vbox">
      <object class="GtkVBox" id="dialog-vbox1">
        <property name="visible">True</property>
        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
        <property name="orientation">vertical</property>
        <property name="spacing">6</property>
        <child>
          <object class="GtkVBox" id="vbox2">
            <property name="visible">True</property>
            <property name="orientation">vertical</property>
            <property name="spacing">6</property>
            <child>
              <object class="GtkLabel" id="rom_name_lbl">
                <property name="visible">True</property>
                <property name="xalign">0</property>
                <property name="label" translatable="yes">label</property>
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
                  <object class="GtkNotebook" id="notebook1">
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="tab_hborder">6</property>
                    <child>
                      <object class="GtkVBox" id="vbox1">
                        <property name="visible">True</property>
                        <property name="border_width">6</property>
                        <property name="orientation">vertical</property>
                        <property name="spacing">6</property>
                        <child>
                          <object class="GtkLabel" id="label13">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">&lt;b&gt;Main Details&lt;/b&gt;</property>
                            <property name="use_markup">True</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="fill">False</property>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkAlignment" id="alignment6">
                            <property name="visible">True</property>
                            <property name="left_padding">12</property>
                            <child>
                              <object class="GtkTable" id="table4">
                                <property name="visible">True</property>
                                <property name="n_rows">5</property>
                                <property name="n_columns">2</property>
                                <property name="column_spacing">6</property>
                                <property name="row_spacing">6</property>
                                <child>
                                  <object class="GtkLabel" id="label12">
                                    <property name="visible">True</property>
                                    <property name="xalign">0</property>
                                    <property name="label" translatable="yes">Romset Name:</property>
                                  </object>
                                  <packing>
                                    <property name="x_options"></property>
                                    <property name="y_options"></property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="lbl_details-romname">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="wrap">True</property>
                                  </object>
                                  <packing>
                                    <property name="left_attach">1</property>
                                    <property name="right_attach">2</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="manufacturer_result">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="wrap">True</property>
                                  </object>
                                  <packing>
                                    <property name="left_attach">1</property>
                                    <property name="right_attach">2</property>
                                    <property name="top_attach">3</property>
                                    <property name="bottom_attach">4</property>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="clone_label">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="label" translatable="yes">Clone of:</property>
                                    <property name="use_markup">True</property>
                                  </object>
                                  <packing>
                                    <property name="top_attach">2</property>
                                    <property name="bottom_attach">3</property>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options"></property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="label17">
                                    <property name="visible">True</property>
                                    <property name="xalign">0</property>
                                    <property name="label" translatable="yes">Driver:</property>
                                  </object>
                                  <packing>
                                    <property name="top_attach">1</property>
                                    <property name="bottom_attach">2</property>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options"></property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="clone_result">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="wrap">True</property>
                                  </object>
                                  <packing>
                                    <property name="left_attach">1</property>
                                    <property name="right_attach">2</property>
                                    <property name="top_attach">2</property>
                                    <property name="bottom_attach">3</property>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="lbl_details-driver">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="wrap">True</property>
                                  </object>
                                  <packing>
                                    <property name="left_attach">1</property>
                                    <property name="right_attach">2</property>
                                    <property name="top_attach">1</property>
                                    <property name="bottom_attach">2</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="label6">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="label" translatable="yes">Manufacturer:</property>
                                    <property name="use_markup">True</property>
                                  </object>
                                  <packing>
                                    <property name="top_attach">3</property>
                                    <property name="bottom_attach">4</property>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options"></property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="label5">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="label" translatable="yes">Year:</property>
                                    <property name="use_markup">True</property>
                                  </object>
                                  <packing>
                                    <property name="top_attach">4</property>
                                    <property name="bottom_attach">5</property>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options"></property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="year_result">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="wrap">True</property>
                                  </object>
                                  <packing>
                                    <property name="left_attach">1</property>
                                    <property name="right_attach">2</property>
                                    <property name="top_attach">4</property>
                                    <property name="bottom_attach">5</property>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options">GTK_FILL</property>
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
                        <child>
                          <object class="GtkLabel" id="label7">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">&lt;b&gt;Technical Details&lt;/b&gt;</property>
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
                                <property name="n_rows">2</property>
                                <property name="n_columns">2</property>
                                <property name="column_spacing">6</property>
                                <property name="row_spacing">6</property>
                                <child>
                                  <object class="GtkLabel" id="label8">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="yalign">0</property>
                                    <property name="label" translatable="yes">CPU:</property>
                                    <property name="use_markup">True</property>
                                  </object>
                                  <packing>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="label9">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="yalign">0</property>
                                    <property name="label" translatable="yes">Sound:</property>
                                    <property name="use_markup">True</property>
                                  </object>
                                  <packing>
                                    <property name="top_attach">1</property>
                                    <property name="bottom_attach">2</property>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="cpu_result">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="wrap">True</property>
                                  </object>
                                  <packing>
                                    <property name="left_attach">1</property>
                                    <property name="right_attach">2</property>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="sound_result">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="wrap">True</property>
                                  </object>
                                  <packing>
                                    <property name="left_attach">1</property>
                                    <property name="right_attach">2</property>
                                    <property name="top_attach">1</property>
                                    <property name="bottom_attach">2</property>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                              </object>
                            </child>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="fill">False</property>
                            <property name="position">3</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="label14">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">&lt;b&gt;Screen Details&lt;/b&gt;</property>
                            <property name="use_markup">True</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="fill">False</property>
                            <property name="position">4</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkAlignment" id="alignment4">
                            <property name="visible">True</property>
                            <property name="left_padding">12</property>
                            <child>
                              <object class="GtkTable" id="table2">
                                <property name="visible">True</property>
                                <property name="n_rows">3</property>
                                <property name="n_columns">2</property>
                                <property name="column_spacing">6</property>
                                <property name="row_spacing">6</property>
                                <child>
                                  <object class="GtkLabel" id="screen_result">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="wrap">True</property>
                                  </object>
                                  <packing>
                                    <property name="left_attach">1</property>
                                    <property name="right_attach">2</property>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="label16">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="label" translatable="yes">Colors:</property>
                                    <property name="use_markup">True</property>
                                  </object>
                                  <packing>
                                    <property name="top_attach">2</property>
                                    <property name="bottom_attach">3</property>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="colors_result">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="wrap">True</property>
                                  </object>
                                  <packing>
                                    <property name="left_attach">1</property>
                                    <property name="right_attach">2</property>
                                    <property name="top_attach">2</property>
                                    <property name="bottom_attach">3</property>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="label15">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="label" translatable="yes">Screen:</property>
                                    <property name="use_markup">True</property>
                                  </object>
                                  <packing>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="label23">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="label" translatable="yes">Type:</property>
                                    <property name="use_markup">True</property>
                                  </object>
                                  <packing>
                                    <property name="top_attach">1</property>
                                    <property name="bottom_attach">2</property>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="screentype_result">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="wrap">True</property>
                                  </object>
                                  <packing>
                                    <property name="left_attach">1</property>
                                    <property name="right_attach">2</property>
                                    <property name="top_attach">1</property>
                                    <property name="bottom_attach">2</property>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                              </object>
                            </child>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="fill">False</property>
                            <property name="position">5</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="label24">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">&lt;b&gt;Input Details&lt;/b&gt;</property>
                            <property name="use_markup">True</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="fill">False</property>
                            <property name="position">6</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkAlignment" id="alignment2">
                            <property name="visible">True</property>
                            <property name="left_padding">12</property>
                            <child>
                              <object class="GtkTable" id="table6">
                                <property name="visible">True</property>
                                <property name="n_rows">3</property>
                                <property name="n_columns">2</property>
                                <property name="column_spacing">6</property>
                                <property name="row_spacing">6</property>
                                <child>
                                  <object class="GtkLabel" id="label25">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="label" translatable="yes">Players:</property>
                                    <property name="use_markup">True</property>
                                  </object>
                                  <packing>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="label26">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="label" translatable="yes">Buttons:</property>
                                    <property name="use_markup">True</property>
                                  </object>
                                  <packing>
                                    <property name="top_attach">1</property>
                                    <property name="bottom_attach">2</property>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="lbl_ctrl-num-players">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="wrap">True</property>
                                  </object>
                                  <packing>
                                    <property name="left_attach">1</property>
                                    <property name="right_attach">2</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="lbl_ctrl-num-buttons">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="wrap">True</property>
                                  </object>
                                  <packing>
                                    <property name="left_attach">1</property>
                                    <property name="right_attach">2</property>
                                    <property name="top_attach">1</property>
                                    <property name="bottom_attach">2</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="label27">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="label" translatable="yes">Type:</property>
                                    <property name="use_markup">True</property>
                                  </object>
                                  <packing>
                                    <property name="top_attach">2</property>
                                    <property name="bottom_attach">3</property>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="lbl_ctrl-control-type">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="wrap">True</property>
                                  </object>
                                  <packing>
                                    <property name="left_attach">1</property>
                                    <property name="right_attach">2</property>
                                    <property name="top_attach">2</property>
                                    <property name="bottom_attach">3</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                              </object>
                            </child>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="fill">False</property>
                            <property name="position">7</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="label4">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">&lt;b&gt;Audit Details&lt;/b&gt;</property>
                            <property name="use_markup">True</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="fill">False</property>
                            <property name="position">8</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkAlignment" id="alignment5">
                            <property name="visible">True</property>
                            <property name="left_padding">12</property>
                            <child>
                              <object class="GtkTable" id="table3">
                                <property name="visible">True</property>
                                <property name="n_rows">2</property>
                                <property name="n_columns">2</property>
                                <property name="column_spacing">6</property>
                                <property name="row_spacing">6</property>
                                <child>
                                  <object class="GtkLabel" id="label10">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="label" translatable="yes">Rom check:</property>
                                    <property name="use_markup">True</property>
                                  </object>
                                  <packing>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="label11">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="label" translatable="yes">Sample Check:</property>
                                    <property name="use_markup">True</property>
                                  </object>
                                  <packing>
                                    <property name="top_attach">1</property>
                                    <property name="bottom_attach">2</property>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="rom_check_result">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="label" translatable="yes">Checking...</property>
                                    <property name="wrap">True</property>
                                  </object>
                                  <packing>
                                    <property name="left_attach">1</property>
                                    <property name="right_attach">2</property>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkLabel" id="sample_check_result">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="xalign">0</property>
                                    <property name="label" translatable="yes">None required</property>
                                    <property name="wrap">True</property>
                                  </object>
                                  <packing>
                                    <property name="left_attach">1</property>
                                    <property name="right_attach">2</property>
                                    <property name="top_attach">1</property>
                                    <property name="bottom_attach">2</property>
                                    <property name="x_options">GTK_FILL</property>
                                    <property name="y_options">GTK_FILL</property>
                                  </packing>
                                </child>
                              </object>
                            </child>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="fill">False</property>
                            <property name="position">9</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkHButtonBox" id="hbuttonbox1">
                            <property name="visible">True</property>
                            <property name="spacing">6</property>
                            <property name="layout_style">end</property>
                            <child>
                              <object class="GtkButton" id="btn_open_rom">
                                <property name="label" translatable="yes">Open ROM</property>
                                <property name="visible">True</property>
                                <property name="can_focus">True</property>
                                <property name="receives_default">True</property>
                                <property name="image">img_open_rom</property>
                              </object>
                              <packing>
                                <property name="expand">False</property>
                                <property name="fill">False</property>
                                <property name="padding">6</property>
                                <property name="position">0</property>
                              </packing>
                            </child>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="fill">False</property>
                            <property name="position">10</property>
                          </packing>
                        </child>
                      </object>
                    </child>
                    <child type="tab">
                      <object class="GtkLabel" id="label1">
                        <property name="visible">True</property>
                        <property name="label" translatable="yes">About this ROM</property>
                      </object>
                      <packing>
                        <property name="tab_fill">False</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkScrolledWindow" id="scrolledwindow_audit">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="border_width">6</property>
                        <property name="hscrollbar_policy">automatic</property>
                      </object>
                      <packing>
                        <property name="position">1</property>
                      </packing>
                    </child>
                    <child type="tab">
                      <object class="GtkLabel" id="label2">
                        <property name="visible">True</property>
                        <property name="label" translatable="yes">Audit Details</property>
                      </object>
                      <packing>
                        <property name="position">1</property>
                        <property name="tab_fill">False</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkVBox" id="vbox6">
                        <property name="visible">True</property>
                        <property name="border_width">6</property>
                        <property name="orientation">vertical</property>
                        <property name="spacing">6</property>
                        <child>
                          <object class="GtkLabel" id="label33">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">&lt;b&gt;Emulation Status&lt;/b&gt;</property>
                            <property name="use_markup">True</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="fill">False</property>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkAlignment" id="alignment9">
                            <property name="visible">True</property>
                            <property name="left_padding">12</property>
                            <child>
                              <object class="GtkVBox" id="vbox7">
                                <property name="visible">True</property>
                                <property name="orientation">vertical</property>
                                <property name="spacing">6</property>
                                <child>
                                  <object class="GtkLabel" id="label34">
                                    <property name="visible">True</property>
                                    <property name="xalign">0</property>
                                    <property name="label" translatable="yes">The ROM has the following emulation support:</property>
                                  </object>
                                  <packing>
                                    <property name="expand">False</property>
                                    <property name="fill">False</property>
                                    <property name="position">0</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkTable" id="table5">
                                    <property name="visible">True</property>
                                    <property name="n_rows">5</property>
                                    <property name="n_columns">2</property>
                                    <property name="column_spacing">6</property>
                                    <property name="row_spacing">6</property>
                                    <child>
                                      <object class="GtkLabel" id="label18">
                                        <property name="visible">True</property>
                                        <property name="xalign">0</property>
                                        <property name="label" translatable="yes">Driver:</property>
                                      </object>
                                      <packing>
                                        <property name="x_options">GTK_FILL</property>
                                        <property name="y_options"></property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="label19">
                                        <property name="visible">True</property>
                                        <property name="xalign">0</property>
                                        <property name="label" translatable="yes">Emulation:</property>
                                      </object>
                                      <packing>
                                        <property name="top_attach">1</property>
                                        <property name="bottom_attach">2</property>
                                        <property name="x_options">GTK_FILL</property>
                                        <property name="y_options"></property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="label20">
                                        <property name="visible">True</property>
                                        <property name="xalign">0</property>
                                        <property name="label" translatable="yes">Graphics:</property>
                                      </object>
                                      <packing>
                                        <property name="top_attach">2</property>
                                        <property name="bottom_attach">3</property>
                                        <property name="x_options">GTK_FILL</property>
                                        <property name="y_options"></property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="label21">
                                        <property name="visible">True</property>
                                        <property name="xalign">0</property>
                                        <property name="label" translatable="yes">Colour:</property>
                                      </object>
                                      <packing>
                                        <property name="top_attach">3</property>
                                        <property name="bottom_attach">4</property>
                                        <property name="x_options">GTK_FILL</property>
                                        <property name="y_options"></property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="label22">
                                        <property name="visible">True</property>
                                        <property name="xalign">0</property>
                                        <property name="label" translatable="yes">Sound:</property>
                                      </object>
                                      <packing>
                                        <property name="top_attach">4</property>
                                        <property name="bottom_attach">5</property>
                                        <property name="x_options">GTK_FILL</property>
                                        <property name="y_options"></property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="lbl_driver-status">
                                        <property name="visible">True</property>
                                        <property name="xalign">0</property>
                                        <property name="label" translatable="yes">label</property>
                                      </object>
                                      <packing>
                                        <property name="left_attach">1</property>
                                        <property name="right_attach">2</property>
                                        <property name="y_options"></property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="lbl_driver-status-emulation">
                                        <property name="visible">True</property>
                                        <property name="xalign">0</property>
                                        <property name="label" translatable="yes">label</property>
                                      </object>
                                      <packing>
                                        <property name="left_attach">1</property>
                                        <property name="right_attach">2</property>
                                        <property name="top_attach">1</property>
                                        <property name="bottom_attach">2</property>
                                        <property name="y_options"></property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="lbl_driver-status-graphics">
                                        <property name="visible">True</property>
                                        <property name="xalign">0</property>
                                        <property name="label" translatable="yes">label</property>
                                      </object>
                                      <packing>
                                        <property name="left_attach">1</property>
                                        <property name="right_attach">2</property>
                                        <property name="top_attach">2</property>
                                        <property name="bottom_attach">3</property>
                                        <property name="y_options"></property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="lbl_driver-status-colour">
                                        <property name="visible">True</property>
                                        <property name="xalign">0</property>
                                        <property name="label" translatable="yes">label</property>
                                      </object>
                                      <packing>
                                        <property name="left_attach">1</property>
                                        <property name="right_attach">2</property>
                                        <property name="top_attach">3</property>
                                        <property name="bottom_attach">4</property>
                                        <property name="y_options"></property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="lbl_driver-status-sound">
                                        <property name="visible">True</property>
                                        <property name="xalign">0</property>
                                        <property name="label" translatable="yes">label</property>
                                      </object>
                                      <packing>
                                        <property name="left_attach">1</property>
                                        <property name="right_attach">2</property>
                                        <property name="top_attach">4</property>
                                        <property name="bottom_attach">5</property>
                                        <property name="y_options"></property>
                                      </packing>
                                    </child>
                                  </object>
                                  <packing>
                                    <property name="expand">False</property>
                                    <property name="fill">False</property>
                                    <property name="position">1</property>
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
                        <property name="position">2</property>
                      </packing>
                    </child>
                    <child type="tab">
                      <object class="GtkLabel" id="label3">
                        <property name="visible">True</property>
                        <property name="label" translatable="yes">Emulation Status</property>
                      </object>
                      <packing>
                        <property name="position">2</property>
                        <property name="tab_fill">False</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkVBox" id="vbox3">
                        <property name="visible">True</property>
                        <property name="border_width">6</property>
                        <property name="orientation">vertical</property>
                        <property name="spacing">6</property>
                        <child>
                          <object class="GtkLabel" id="label29">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">&lt;b&gt;Brothers&lt;/b&gt;</property>
                            <property name="use_markup">True</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="fill">False</property>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkAlignment" id="alignment7">
                            <property name="visible">True</property>
                            <property name="left_padding">12</property>
                            <child>
                              <object class="GtkVBox" id="vbox4">
                                <property name="visible">True</property>
                                <property name="orientation">vertical</property>
                                <property name="spacing">6</property>
                                <child>
                                  <object class="GtkLabel" id="label31">
                                    <property name="visible">True</property>
                                    <property name="xalign">0</property>
                                    <property name="label" translatable="yes">The following ROMs are supported by same driver:</property>
                                  </object>
                                  <packing>
                                    <property name="expand">False</property>
                                    <property name="fill">False</property>
                                    <property name="position">0</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkScrolledWindow" id="scrolledwindow_brothers">
                                    <property name="height_request">200</property>
                                    <property name="visible">True</property>
                                    <property name="can_focus">True</property>
                                    <property name="hscrollbar_policy">automatic</property>
                                  </object>
                                  <packing>
                                    <property name="expand">False</property>
                                    <property name="fill">False</property>
                                    <property name="position">1</property>
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
                        <child>
                          <object class="GtkLabel" id="label30">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">&lt;b&gt;Clones&lt;/b&gt;</property>
                            <property name="use_markup">True</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="fill">False</property>
                            <property name="position">2</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkAlignment" id="alignment8">
                            <property name="visible">True</property>
                            <property name="left_padding">12</property>
                            <child>
                              <object class="GtkVBox" id="vbox5">
                                <property name="visible">True</property>
                                <property name="orientation">vertical</property>
                                <property name="spacing">6</property>
                                <child>
                                  <object class="GtkLabel" id="label32">
                                    <property name="visible">True</property>
                                    <property name="xalign">0</property>
                                    <property name="label" translatable="yes">The following ROMs are clones of this ROM:</property>
                                  </object>
                                  <packing>
                                    <property name="expand">False</property>
                                    <property name="fill">False</property>
                                    <property name="position">0</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkScrolledWindow" id="scrolledwindow_clones">
                                    <property name="height_request">200</property>
                                    <property name="visible">True</property>
                                    <property name="can_focus">True</property>
                                    <property name="hscrollbar_policy">automatic</property>
                                  </object>
                                  <packing>
                                    <property name="expand">False</property>
                                    <property name="fill">False</property>
                                    <property name="position">1</property>
                                  </packing>
                                </child>
                              </object>
                            </child>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="fill">False</property>
                            <property name="position">3</property>
                          </packing>
                        </child>
                      </object>
                      <packing>
                        <property name="position">3</property>
                      </packing>
                    </child>
                    <child type="tab">
                      <object class="GtkLabel" id="label28">
                        <property name="visible">True</property>
                        <property name="label" translatable="yes">Associated ROMs</property>
                      </object>
                      <packing>
                        <property name="position">3</property>
                        <property name="tab_fill">False</property>
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
            <property name="position">1</property>
          </packing>
        </child>
        <child internal-child="action_area">
          <object class="GtkHButtonBox" id="dialog-action_area1">
            <property name="visible">True</property>
            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
            <property name="layout_style">end</property>
            <child>
              <object class="GtkButton" id="button1">
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
      <action-widget response="0">button1</action-widget>
    </action-widgets>
  </object>
  <object class="GtkAdjustment" id="adjustment1">
    <property name="upper">100</property>
    <property name="step_increment">1</property>
    <property name="page_increment">10</property>
    <property name="page_size">10</property>
  </object>
  <object class="GtkAdjustment" id="adjustment2">
    <property name="upper">100</property>
    <property name="step_increment">1</property>
    <property name="page_increment">10</property>
    <property name="page_size">10</property>
  </object>
  <object class="GtkImage" id="img_open_rom">
    <property name="visible">True</property>
    <property name="stock">gtk-jump-to</property>
  </object>
</interface>
