<?xml version="1.0"?>
<!--Generated with glade3 3.4.5 on Mon Aug 18 22:44:46 2008 -->
<interface>
  <object class="GtkDialog" id="checking_games_window">
    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
    <property name="border_width">5</property>
    <property name="title" translatable="yes">Checking Games</property>
    <property name="modal">True</property>
    <property name="window_position">GTK_WIN_POS_CENTER_ON_PARENT</property>
    <property name="type_hint">GDK_WINDOW_TYPE_HINT_DIALOG</property>
    <property name="has_separator">False</property>
    <child internal-child="vbox">
      <object class="GtkVBox" id="dialog-vbox1">
        <property name="visible">True</property>
        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
        <property name="spacing">2</property>
        <child>
          <object class="GtkVBox" id="vbox1">
            <property name="visible">True</property>
            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
            <property name="spacing">6</property>
            <child>
              <object class="GtkLabel" id="label1">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="xalign">0</property>
                <property name="label" translatable="yes">Auditing ROMs</property>
                <property name="use_markup">True</property>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="padding">6</property>
              </packing>
            </child>
            <child>
              <object class="GtkLabel" id="label5">
                <property name="visible">True</property>
                <property name="xalign">0</property>
                <property name="label" translatable="yes">GMAMEUI is now auditing the ROMs in the specified ROM paths. This may take some time.</property>
                <property name="wrap">True</property>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="padding">6</property>
                <property name="position">1</property>
              </packing>
            </child>
            <child>
              <object class="GtkProgressBar" id="progressbar1">
                <property name="visible">True</property>
                <property name="text" translatable="yes"/>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="position">2</property>
              </packing>
            </child>
            <child>
              <object class="GtkAlignment" id="alignment1">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="xalign">0</property>
                <property name="left_padding">6</property>
                <child>
                  <object class="GtkLabel" id="checking_games_label">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="xalign">0</property>
                    <property name="label" translatable="yes">Auditing MAME ROMs</property>
                    <property name="use_markup">True</property>
                  </object>
                </child>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="position">3</property>
              </packing>
            </child>
            <child>
              <object class="GtkExpander" id="expander1">
                <property name="visible">True</property>
                <property name="can_focus">True</property>
                <property name="spacing">6</property>
                <child>
                  <object class="GtkVBox" id="vbox3">
                    <property name="visible">True</property>
                    <property name="spacing">6</property>
                    <child>
                      <object class="GtkHBox" id="hbox1">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="homogeneous">True</property>
                        <child>
                          <object class="GtkVBox" id="vbox2">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="spacing">6</property>
                            <child>
                              <object class="GtkLabel" id="label2">
                                <property name="visible">True</property>
                                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                <property name="xalign">0</property>
                                <property name="label" translatable="yes">&lt;b&gt;ROMs&lt;/b&gt;</property>
                                <property name="use_markup">True</property>
                              </object>
                              <packing>
                                <property name="expand">False</property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkAlignment" id="alignment4">
                                <property name="visible">True</property>
                                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                <property name="left_padding">12</property>
                                <child>
                                  <object class="GtkTable" id="table2">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="n_rows">5</property>
                                    <property name="n_columns">2</property>
                                    <property name="column_spacing">6</property>
                                    <property name="row_spacing">6</property>
                                    <child>
                                      <object class="GtkLabel" id="correct_roms_label">
                                        <property name="visible">True</property>
                                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                        <property name="xalign">0</property>
                                        <property name="label" translatable="yes">Correct:</property>
                                      </object>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="bestavailable_roms_label">
                                        <property name="visible">True</property>
                                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                        <property name="xalign">0</property>
                                        <property name="label" translatable="yes">Best available:</property>
                                      </object>
                                      <packing>
                                        <property name="top_attach">1</property>
                                        <property name="bottom_attach">2</property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="incorrect_roms_label">
                                        <property name="visible">True</property>
                                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                        <property name="xalign">0</property>
                                        <property name="label" translatable="yes">Incorrect:</property>
                                      </object>
                                      <packing>
                                        <property name="top_attach">2</property>
                                        <property name="bottom_attach">3</property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="notfound_roms_label">
                                        <property name="visible">True</property>
                                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                        <property name="xalign">0</property>
                                        <property name="label" translatable="yes">Not found:</property>
                                      </object>
                                      <packing>
                                        <property name="top_attach">3</property>
                                        <property name="bottom_attach">4</property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="total_roms_label">
                                        <property name="visible">True</property>
                                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                        <property name="xalign">0</property>
                                        <property name="label" translatable="yes">Total:</property>
                                      </object>
                                      <packing>
                                        <property name="top_attach">4</property>
                                        <property name="bottom_attach">5</property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="correct_roms_value">
                                        <property name="visible">True</property>
                                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                        <property name="xalign">1</property>
                                        <property name="label" translatable="yes">0</property>
                                      </object>
                                      <packing>
                                        <property name="left_attach">1</property>
                                        <property name="right_attach">2</property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="bestavailable_roms_value">
                                        <property name="visible">True</property>
                                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                        <property name="xalign">1</property>
                                        <property name="label" translatable="yes">0</property>
                                      </object>
                                      <packing>
                                        <property name="left_attach">1</property>
                                        <property name="right_attach">2</property>
                                        <property name="top_attach">1</property>
                                        <property name="bottom_attach">2</property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="incorrect_roms_value">
                                        <property name="visible">True</property>
                                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                        <property name="xalign">1</property>
                                        <property name="label" translatable="yes">0</property>
                                      </object>
                                      <packing>
                                        <property name="left_attach">1</property>
                                        <property name="right_attach">2</property>
                                        <property name="top_attach">2</property>
                                        <property name="bottom_attach">3</property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="notfound_roms_value">
                                        <property name="visible">True</property>
                                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                        <property name="xalign">1</property>
                                        <property name="label" translatable="yes">0</property>
                                      </object>
                                      <packing>
                                        <property name="left_attach">1</property>
                                        <property name="right_attach">2</property>
                                        <property name="top_attach">3</property>
                                        <property name="bottom_attach">4</property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="total_roms_value">
                                        <property name="visible">True</property>
                                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                        <property name="xalign">1</property>
                                        <property name="label" translatable="yes">0</property>
                                      </object>
                                      <packing>
                                        <property name="left_attach">1</property>
                                        <property name="right_attach">2</property>
                                        <property name="top_attach">4</property>
                                        <property name="bottom_attach">5</property>
                                      </packing>
                                    </child>
                                  </object>
                                </child>
                              </object>
                              <packing>
                                <property name="expand">False</property>
                                <property name="position">1</property>
                              </packing>
                            </child>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="padding">6</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkVBox" id="vbox4">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="spacing">6</property>
                            <child>
                              <object class="GtkLabel" id="label27">
                                <property name="visible">True</property>
                                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                <property name="xalign">0</property>
                                <property name="label" translatable="yes">&lt;b&gt;Samples&lt;/b&gt;</property>
                                <property name="use_markup">True</property>
                              </object>
                              <packing>
                                <property name="expand">False</property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkAlignment" id="alignment5">
                                <property name="visible">True</property>
                                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                <property name="left_padding">12</property>
                                <child>
                                  <object class="GtkTable" id="table4">
                                    <property name="visible">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="n_rows">4</property>
                                    <property name="n_columns">2</property>
                                    <property name="column_spacing">6</property>
                                    <property name="row_spacing">6</property>
                                    <child>
                                      <object class="GtkLabel" id="notfound_samples_value">
                                        <property name="visible">True</property>
                                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                        <property name="xalign">1</property>
                                        <property name="label" translatable="yes">0</property>
                                      </object>
                                      <packing>
                                        <property name="left_attach">1</property>
                                        <property name="right_attach">2</property>
                                        <property name="top_attach">2</property>
                                        <property name="bottom_attach">3</property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="notfound_samples_label">
                                        <property name="visible">True</property>
                                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                        <property name="xalign">0</property>
                                        <property name="label" translatable="yes">Not found:</property>
                                      </object>
                                      <packing>
                                        <property name="top_attach">2</property>
                                        <property name="bottom_attach">3</property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="total_samples_value">
                                        <property name="visible">True</property>
                                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                        <property name="xalign">1</property>
                                        <property name="label" translatable="yes">0</property>
                                      </object>
                                      <packing>
                                        <property name="left_attach">1</property>
                                        <property name="right_attach">2</property>
                                        <property name="top_attach">3</property>
                                        <property name="bottom_attach">4</property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="total_samples_label">
                                        <property name="visible">True</property>
                                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                        <property name="xalign">0</property>
                                        <property name="label" translatable="yes">Total:</property>
                                      </object>
                                      <packing>
                                        <property name="top_attach">3</property>
                                        <property name="bottom_attach">4</property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="correct_samples_label">
                                        <property name="visible">True</property>
                                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                        <property name="xalign">0</property>
                                        <property name="label" translatable="yes">Correct:</property>
                                      </object>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="correct_samples_value">
                                        <property name="visible">True</property>
                                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                        <property name="xalign">1</property>
                                        <property name="label" translatable="yes">0</property>
                                      </object>
                                      <packing>
                                        <property name="left_attach">1</property>
                                        <property name="right_attach">2</property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="incorrect_samples_label">
                                        <property name="visible">True</property>
                                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                        <property name="xalign">0</property>
                                        <property name="label" translatable="yes">Incorrect:</property>
                                      </object>
                                      <packing>
                                        <property name="top_attach">1</property>
                                        <property name="bottom_attach">2</property>
                                      </packing>
                                    </child>
                                    <child>
                                      <object class="GtkLabel" id="incorrect_samples_value">
                                        <property name="visible">True</property>
                                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                        <property name="xalign">1</property>
                                        <property name="label" translatable="yes">0</property>
                                      </object>
                                      <packing>
                                        <property name="left_attach">1</property>
                                        <property name="right_attach">2</property>
                                        <property name="top_attach">1</property>
                                        <property name="bottom_attach">2</property>
                                      </packing>
                                    </child>
                                  </object>
                                </child>
                              </object>
                              <packing>
                                <property name="expand">False</property>
                                <property name="position">1</property>
                              </packing>
                            </child>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="padding">6</property>
                            <property name="position">1</property>
                          </packing>
                        </child>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkLabel" id="label3">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">&lt;b&gt;Details&lt;/b&gt;</property>
                        <property name="use_markup">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">1</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkAlignment" id="alignment3">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="left_padding">6</property>
                        <child>
                          <object class="GtkScrolledWindow" id="scrolledwindow1">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="hscrollbar_policy">GTK_POLICY_AUTOMATIC</property>
                            <property name="vscrollbar_policy">GTK_POLICY_AUTOMATIC</property>
                            <property name="shadow_type">GTK_SHADOW_IN</property>
                            <child>
                              <object class="GtkTextView" id="details_check_text">
                                <property name="height_request">300</property>
                                <property name="visible">True</property>
                                <property name="can_focus">True</property>
                                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                <property name="editable">False</property>
                                <property name="cursor_visible">False</property>
                              </object>
                            </child>
                          </object>
                        </child>
                      </object>
                      <packing>
                        <property name="position">2</property>
                      </packing>
                    </child>
                  </object>
                </child>
                <child type="label">
                  <object class="GtkLabel" id="label4">
                    <property name="visible">True</property>
                    <property name="label" translatable="yes">Details</property>
                  </object>
                </child>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="fill">False</property>
                <property name="position">4</property>
              </packing>
            </child>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="fill">False</property>
            <property name="position">1</property>
          </packing>
        </child>
        <child internal-child="action_area">
          <object class="GtkHButtonBox" id="dialog-action_area1">
            <property name="visible">True</property>
            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
            <property name="layout_style">GTK_BUTTONBOX_END</property>
            <child>
              <object class="GtkButton" id="stop_audit_button">
                <property name="visible">True</property>
                <property name="can_focus">True</property>
                <property name="receives_default">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="label" translatable="yes">gtk-stop</property>
                <property name="use_stock">True</property>
              </object>
            </child>
            <child>
              <object class="GtkButton" id="close_audit_button">
                <property name="visible">True</property>
                <property name="can_focus">True</property>
                <property name="receives_default">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="label" translatable="yes">gtk-close</property>
                <property name="use_stock">True</property>
              </object>
              <packing>
                <property name="position">1</property>
              </packing>
            </child>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="pack_type">GTK_PACK_END</property>
          </packing>
        </child>
      </object>
    </child>
    <action-widgets>
      <action-widget response="-2">stop_audit_button</action-widget>
      <action-widget response="-7">close_audit_button</action-widget>
    </action-widgets>
  </object>
</interface>
