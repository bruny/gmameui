<?xml version="1.0"?>
<interface>
  <!-- interface-requires gtk+ 2.12 -->
  <!-- interface-naming-policy toplevel-contextual -->
  <object class="GtkDialog" id="directories_selection">
    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
    <property name="border_width">5</property>
    <property name="title" translatable="yes">Directories Selection</property>
    <property name="window_position">center-on-parent</property>
    <property name="type_hint">dialog</property>
    <property name="has_separator">False</property>
    <child internal-child="vbox">
      <object class="GtkVBox" id="dialog-vbox1">
        <property name="visible">True</property>
        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
        <property name="orientation">vertical</property>
        <property name="spacing">2</property>
        <child>
          <object class="GtkNotebook" id="notebook1">
            <property name="visible">True</property>
            <property name="can_focus">True</property>
            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
            <child>
              <object class="GtkVBox" id="vbox2">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="orientation">vertical</property>
                <property name="spacing">6</property>
                <child>
                  <object class="GtkLabel" id="lbl_heading_mame_execs">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="xalign">0</property>
                    <property name="label" translatable="yes">&lt;b&gt;MAME Executables&lt;/b&gt;</property>
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
                  <object class="GtkAlignment" id="alignment2">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="left_padding">12</property>
                    <child>
                      <object class="GtkTable" id="table3">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="n_rows">6</property>
                        <property name="n_columns">3</property>
                        <property name="column_spacing">6</property>
                        <property name="row_spacing">6</property>
                        <child>
                          <object class="GtkLabel" id="lbl_mame_execs">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">MAME executables</property>
                          </object>
                          <packing>
                            <property name="bottom_attach">2</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkButton" id="xmame_execs_add_button">
                            <property name="label">gtk-add</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="use_stock">True</property>
                          </object>
                          <packing>
                            <property name="left_attach">2</property>
                            <property name="right_attach">3</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkButton" id="xmame_execs_remove_button">
                            <property name="label">gtk-remove</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="use_stock">True</property>
                          </object>
                          <packing>
                            <property name="left_attach">2</property>
                            <property name="right_attach">3</property>
                            <property name="top_attach">1</property>
                            <property name="bottom_attach">2</property>
                            <property name="x_options"></property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkScrolledWindow" id="scrolledwindow3">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="hscrollbar_policy">never</property>
                            <property name="vscrollbar_policy">automatic</property>
                            <child>
                              <object class="GtkViewport" id="viewport3">
                                <property name="visible">True</property>
                                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                <property name="resize_mode">queue</property>
                                <child>
                                  <object class="GtkTreeView" id="xmame_execs_tree_view">
                                    <property name="visible">True</property>
                                    <property name="can_focus">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="headers_visible">False</property>
                                  </object>
                                </child>
                              </object>
                            </child>
                          </object>
                          <packing>
                            <property name="left_attach">1</property>
                            <property name="right_attach">2</property>
                            <property name="bottom_attach">2</property>
                            <property name="y_options">GTK_FILL</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="lbl_roms">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">ROMs</property>
                          </object>
                          <packing>
                            <property name="top_attach">2</property>
                            <property name="bottom_attach">4</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkScrolledWindow" id="scrolledwindow1">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="hscrollbar_policy">never</property>
                            <property name="vscrollbar_policy">automatic</property>
                            <child>
                              <object class="GtkViewport" id="viewport1">
                                <property name="visible">True</property>
                                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                <property name="resize_mode">queue</property>
                                <child>
                                  <object class="GtkTreeView" id="roms_path_tree_view">
                                    <property name="visible">True</property>
                                    <property name="can_focus">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="headers_visible">False</property>
                                  </object>
                                </child>
                              </object>
                            </child>
                          </object>
                          <packing>
                            <property name="left_attach">1</property>
                            <property name="right_attach">2</property>
                            <property name="top_attach">2</property>
                            <property name="bottom_attach">4</property>
                            <property name="y_options">GTK_FILL</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkButton" id="roms_add_button">
                            <property name="label">gtk-add</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="use_stock">True</property>
                          </object>
                          <packing>
                            <property name="left_attach">2</property>
                            <property name="right_attach">3</property>
                            <property name="top_attach">2</property>
                            <property name="bottom_attach">3</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkButton" id="roms_remove_button">
                            <property name="label">gtk-remove</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="use_stock">True</property>
                            <property name="xalign">0</property>
                            <property name="yalign">0</property>
                          </object>
                          <packing>
                            <property name="left_attach">2</property>
                            <property name="right_attach">3</property>
                            <property name="top_attach">3</property>
                            <property name="bottom_attach">4</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="lbl_samples">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Samples</property>
                          </object>
                          <packing>
                            <property name="top_attach">4</property>
                            <property name="bottom_attach">6</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkScrolledWindow" id="scrolledwindow2">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="hscrollbar_policy">never</property>
                            <property name="vscrollbar_policy">automatic</property>
                            <child>
                              <object class="GtkViewport" id="viewport2">
                                <property name="visible">True</property>
                                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                <property name="resize_mode">queue</property>
                                <child>
                                  <object class="GtkTreeView" id="samples_path_tree_view">
                                    <property name="visible">True</property>
                                    <property name="can_focus">True</property>
                                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                    <property name="headers_visible">False</property>
                                  </object>
                                </child>
                              </object>
                            </child>
                          </object>
                          <packing>
                            <property name="left_attach">1</property>
                            <property name="right_attach">2</property>
                            <property name="top_attach">4</property>
                            <property name="bottom_attach">6</property>
                            <property name="y_options">GTK_FILL</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkButton" id="samples_add_button">
                            <property name="label">gtk-add</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="use_stock">True</property>
                          </object>
                          <packing>
                            <property name="left_attach">2</property>
                            <property name="right_attach">3</property>
                            <property name="top_attach">4</property>
                            <property name="bottom_attach">5</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkButton" id="samples_remove_button">
                            <property name="label">gtk-remove</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="use_stock">True</property>
                            <property name="xalign">0</property>
                            <property name="yalign">0</property>
                          </object>
                          <packing>
                            <property name="left_attach">2</property>
                            <property name="right_attach">3</property>
                            <property name="top_attach">5</property>
                            <property name="bottom_attach">6</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
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
            </child>
            <child type="tab">
              <object class="GtkLabel" id="label45">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="label" translatable="yes">MAME Main Paths</property>
              </object>
              <packing>
                <property name="tab_fill">False</property>
              </packing>
            </child>
            <child>
              <object class="GtkVBox" id="vbox3">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="orientation">vertical</property>
                <property name="spacing">6</property>
                <child>
                  <object class="GtkLabel" id="lbl_heading_mame_paths">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="xalign">0</property>
                    <property name="label" translatable="yes">&lt;b&gt;MAME Paths&lt;/b&gt;</property>
                    <property name="use_markup">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="padding">6</property>
                    <property name="position">0</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkAlignment" id="alignment3">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="left_padding">12</property>
                    <child>
                      <object class="GtkTable" id="table2">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="n_rows">7</property>
                        <property name="n_columns">4</property>
                        <property name="column_spacing">6</property>
                        <property name="row_spacing">6</property>
                        <child>
                          <object class="GtkButton" id="button_dir-cpanel">
                            <property name="label" translatable="yes">Browse...</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                          </object>
                          <packing>
                            <property name="left_attach">2</property>
                            <property name="right_attach">3</property>
                            <property name="top_attach">6</property>
                            <property name="bottom_attach">7</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkEntry" id="entry_dir-cpanel">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="tooltip_text">Path containing MAME title screenshots</property>
                          </object>
                          <packing>
                            <property name="left_attach">1</property>
                            <property name="right_attach">2</property>
                            <property name="top_attach">6</property>
                            <property name="bottom_attach">7</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="label5">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Control panels:</property>
                          </object>
                          <packing>
                            <property name="top_attach">6</property>
                            <property name="bottom_attach">7</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkButton" id="button_dir-title">
                            <property name="label" translatable="yes">Browse...</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                          </object>
                          <packing>
                            <property name="left_attach">2</property>
                            <property name="right_attach">3</property>
                            <property name="top_attach">5</property>
                            <property name="bottom_attach">6</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkEntry" id="entry_dir-title">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="tooltip_text">Path containing MAME title screenshots</property>
                          </object>
                          <packing>
                            <property name="left_attach">1</property>
                            <property name="right_attach">2</property>
                            <property name="top_attach">5</property>
                            <property name="bottom_attach">6</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="label22">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Title screenshots:</property>
                          </object>
                          <packing>
                            <property name="top_attach">5</property>
                            <property name="bottom_attach">6</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkButton" id="button_dir-marquee">
                            <property name="label" translatable="yes">Browse...</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                          </object>
                          <packing>
                            <property name="left_attach">2</property>
                            <property name="right_attach">3</property>
                            <property name="top_attach">4</property>
                            <property name="bottom_attach">5</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkEntry" id="entry_dir-marquee">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="tooltip_text">Path containing MAME marquees</property>
                          </object>
                          <packing>
                            <property name="left_attach">1</property>
                            <property name="right_attach">2</property>
                            <property name="top_attach">4</property>
                            <property name="bottom_attach">5</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="label21">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Marquees:</property>
                          </object>
                          <packing>
                            <property name="top_attach">4</property>
                            <property name="bottom_attach">5</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkButton" id="button_dir-cabinet">
                            <property name="label" translatable="yes">Browse...</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                          </object>
                          <packing>
                            <property name="left_attach">2</property>
                            <property name="right_attach">3</property>
                            <property name="top_attach">3</property>
                            <property name="bottom_attach">4</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkEntry" id="entry_dir-cabinet">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="tooltip_text">Path containing MAME cabinets</property>
                          </object>
                          <packing>
                            <property name="left_attach">1</property>
                            <property name="right_attach">2</property>
                            <property name="top_attach">3</property>
                            <property name="bottom_attach">4</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="label20">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Cabinets:</property>
                          </object>
                          <packing>
                            <property name="top_attach">3</property>
                            <property name="bottom_attach">4</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkButton" id="button_dir-flyer">
                            <property name="label" translatable="yes">Browse...</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                          </object>
                          <packing>
                            <property name="left_attach">2</property>
                            <property name="right_attach">3</property>
                            <property name="top_attach">2</property>
                            <property name="bottom_attach">3</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkEntry" id="entry_dir-flyer">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="tooltip_text">Path containing MAME flyers</property>
                          </object>
                          <packing>
                            <property name="left_attach">1</property>
                            <property name="right_attach">2</property>
                            <property name="top_attach">2</property>
                            <property name="bottom_attach">3</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="label19">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Flyers:</property>
                          </object>
                          <packing>
                            <property name="top_attach">2</property>
                            <property name="bottom_attach">3</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkButton" id="button_dir-artwork">
                            <property name="label" translatable="yes">Browse...</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                          </object>
                          <packing>
                            <property name="left_attach">2</property>
                            <property name="right_attach">3</property>
                            <property name="top_attach">1</property>
                            <property name="bottom_attach">2</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkEntry" id="entry_dir-artwork">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="tooltip_text">Path containing MAME artwork</property>
                          </object>
                          <packing>
                            <property name="left_attach">1</property>
                            <property name="right_attach">2</property>
                            <property name="top_attach">1</property>
                            <property name="bottom_attach">2</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="label16">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Artwork:</property>
                          </object>
                          <packing>
                            <property name="top_attach">1</property>
                            <property name="bottom_attach">2</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkButton" id="button_dir-snapshot">
                            <property name="label" translatable="yes">Browse...</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                          </object>
                          <packing>
                            <property name="left_attach">2</property>
                            <property name="right_attach">3</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkEntry" id="entry_dir-snapshot">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                          </object>
                          <packing>
                            <property name="left_attach">1</property>
                            <property name="right_attach">2</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="label42">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Snapshots:</property>
                          </object>
                          <packing>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLinkButton" id="link_dir-flyers">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="relief">none</property>
                          </object>
                          <packing>
                            <property name="left_attach">3</property>
                            <property name="right_attach">4</property>
                            <property name="top_attach">2</property>
                            <property name="bottom_attach">3</property>
                            <property name="x_options"></property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLinkButton" id="link_dir-title">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="relief">none</property>
                          </object>
                          <packing>
                            <property name="left_attach">3</property>
                            <property name="right_attach">4</property>
                            <property name="top_attach">5</property>
                            <property name="bottom_attach">6</property>
                            <property name="x_options"></property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLinkButton" id="link_dir-artwork">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="relief">none</property>
                          </object>
                          <packing>
                            <property name="left_attach">3</property>
                            <property name="right_attach">4</property>
                            <property name="top_attach">1</property>
                            <property name="bottom_attach">2</property>
                            <property name="x_options"></property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <placeholder/>
                        </child>
                        <child>
                          <placeholder/>
                        </child>
                        <child>
                          <placeholder/>
                        </child>
                        <child>
                          <placeholder/>
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
            <child type="tab">
              <object class="GtkLabel" id="label49">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="tooltip_text">Path containing MAME snapshots</property>
                <property name="label" translatable="yes">MAME Paths</property>
              </object>
              <packing>
                <property name="position">1</property>
                <property name="tab_fill">False</property>
              </packing>
            </child>
            <child>
              <object class="GtkVBox" id="vbox4">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="orientation">vertical</property>
                <property name="spacing">6</property>
                <child>
                  <object class="GtkLabel" id="lbl_heading_mame_support">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="xalign">0</property>
                    <property name="label" translatable="yes">&lt;b&gt;MAME Support Files&lt;/b&gt;</property>
                    <property name="use_markup">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="padding">6</property>
                    <property name="position">0</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkAlignment" id="alignment5">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="left_padding">12</property>
                    <child>
                      <object class="GtkTable" id="table5">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="n_rows">7</property>
                        <property name="n_columns">4</property>
                        <property name="column_spacing">6</property>
                        <property name="row_spacing">6</property>
                        <child>
                          <object class="GtkButton" id="button_file-catver">
                            <property name="label" translatable="yes">Browse...</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                          </object>
                          <packing>
                            <property name="left_attach">2</property>
                            <property name="right_attach">3</property>
                            <property name="top_attach">6</property>
                            <property name="bottom_attach">7</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkEntry" id="entry_file-catver">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="tooltip_text">Path containing MAME catver.ini</property>
                            <property name="editable">False</property>
                          </object>
                          <packing>
                            <property name="left_attach">1</property>
                            <property name="right_attach">2</property>
                            <property name="top_attach">6</property>
                            <property name="bottom_attach">7</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="label18">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Catver file:</property>
                          </object>
                          <packing>
                            <property name="top_attach">6</property>
                            <property name="bottom_attach">7</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkEntry" id="entry_dir-ctrlr">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="editable">False</property>
                          </object>
                          <packing>
                            <property name="left_attach">1</property>
                            <property name="right_attach">2</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkButton" id="button_dir-ctrlr">
                            <property name="label" translatable="yes">Browse...</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                          </object>
                          <packing>
                            <property name="left_attach">2</property>
                            <property name="right_attach">3</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="label48">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Controller directory:</property>
                          </object>
                          <packing>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkButton" id="button_dir-icons">
                            <property name="label" translatable="yes">Browse...</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                          </object>
                          <packing>
                            <property name="left_attach">2</property>
                            <property name="right_attach">3</property>
                            <property name="top_attach">1</property>
                            <property name="bottom_attach">2</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkEntry" id="entry_dir-icons">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="editable">False</property>
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
                          <object class="GtkLabel" id="label23">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Icons directory:</property>
                          </object>
                          <packing>
                            <property name="top_attach">1</property>
                            <property name="bottom_attach">2</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkButton" id="button_file-history">
                            <property name="label" translatable="yes">Browse...</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                          </object>
                          <packing>
                            <property name="left_attach">2</property>
                            <property name="right_attach">3</property>
                            <property name="top_attach">3</property>
                            <property name="bottom_attach">4</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkEntry" id="entry_file-history">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="tooltip_text">Path to MAME history.dat file</property>
                            <property name="editable">False</property>
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
                          <object class="GtkLabel" id="label43">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">History file:</property>
                          </object>
                          <packing>
                            <property name="top_attach">3</property>
                            <property name="bottom_attach">4</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkButton" id="button_file-mameinfo">
                            <property name="label" translatable="yes">Browse...</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                          </object>
                          <packing>
                            <property name="left_attach">2</property>
                            <property name="right_attach">3</property>
                            <property name="top_attach">2</property>
                            <property name="bottom_attach">3</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkEntry" id="entry_file-mameinfo">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="editable">False</property>
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
                          <object class="GtkButton" id="button_file-hiscore">
                            <property name="label" translatable="yes">Browse...</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                          </object>
                          <packing>
                            <property name="left_attach">2</property>
                            <property name="right_attach">3</property>
                            <property name="top_attach">5</property>
                            <property name="bottom_attach">6</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkEntry" id="entry_file-hiscore">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="editable">False</property>
                          </object>
                          <packing>
                            <property name="left_attach">1</property>
                            <property name="right_attach">2</property>
                            <property name="top_attach">5</property>
                            <property name="bottom_attach">6</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkEntry" id="entry_file-cheat">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="editable">False</property>
                          </object>
                          <packing>
                            <property name="left_attach">1</property>
                            <property name="right_attach">2</property>
                            <property name="top_attach">4</property>
                            <property name="bottom_attach">5</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkButton" id="button_file-cheat">
                            <property name="label" translatable="yes">Browse...</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                          </object>
                          <packing>
                            <property name="left_attach">2</property>
                            <property name="right_attach">3</property>
                            <property name="top_attach">4</property>
                            <property name="bottom_attach">5</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="label44">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">MAME info file:</property>
                          </object>
                          <packing>
                            <property name="top_attach">2</property>
                            <property name="bottom_attach">3</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="label46">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Cheat file:</property>
                          </object>
                          <packing>
                            <property name="top_attach">4</property>
                            <property name="bottom_attach">5</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="label47">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">High score file:</property>
                          </object>
                          <packing>
                            <property name="top_attach">5</property>
                            <property name="bottom_attach">6</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLinkButton" id="link_dir-icons">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="relief">none</property>
                          </object>
                          <packing>
                            <property name="left_attach">3</property>
                            <property name="right_attach">4</property>
                            <property name="top_attach">1</property>
                            <property name="bottom_attach">2</property>
                            <property name="x_options"></property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLinkButton" id="link_file-mameinfo">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="relief">none</property>
                          </object>
                          <packing>
                            <property name="left_attach">3</property>
                            <property name="right_attach">4</property>
                            <property name="top_attach">2</property>
                            <property name="bottom_attach">3</property>
                            <property name="x_options"></property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLinkButton" id="link_file-history">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="relief">none</property>
                          </object>
                          <packing>
                            <property name="left_attach">3</property>
                            <property name="right_attach">4</property>
                            <property name="top_attach">3</property>
                            <property name="bottom_attach">4</property>
                            <property name="x_options"></property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLinkButton" id="link_file-cheat">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="relief">none</property>
                          </object>
                          <packing>
                            <property name="left_attach">3</property>
                            <property name="right_attach">4</property>
                            <property name="top_attach">4</property>
                            <property name="bottom_attach">5</property>
                            <property name="x_options"></property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLinkButton" id="link_file-hiscore">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="relief">none</property>
                          </object>
                          <packing>
                            <property name="left_attach">3</property>
                            <property name="right_attach">4</property>
                            <property name="top_attach">5</property>
                            <property name="bottom_attach">6</property>
                            <property name="x_options"></property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLinkButton" id="link_file-catver">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">True</property>
                            <property name="relief">none</property>
                          </object>
                          <packing>
                            <property name="left_attach">3</property>
                            <property name="right_attach">4</property>
                            <property name="top_attach">6</property>
                            <property name="bottom_attach">7</property>
                            <property name="x_options"></property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <placeholder/>
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
                <property name="position">2</property>
              </packing>
            </child>
            <child type="tab">
              <object class="GtkLabel" id="label12">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="label" translatable="yes">MAME Support Files</property>
              </object>
              <packing>
                <property name="position">2</property>
                <property name="tab_fill">False</property>
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
            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
            <property name="layout_style">end</property>
            <child>
              <object class="GtkButton" id="button1">
                <property name="label">gtk-cancel</property>
                <property name="visible">True</property>
                <property name="can_focus">True</property>
                <property name="receives_default">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="use_stock">True</property>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="fill">False</property>
                <property name="position">0</property>
              </packing>
            </child>
            <child>
              <object class="GtkButton" id="button2">
                <property name="label">gtk-ok</property>
                <property name="visible">True</property>
                <property name="can_focus">True</property>
                <property name="can_default">True</property>
                <property name="has_default">True</property>
                <property name="receives_default">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="use_stock">True</property>
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
            <property name="pack_type">end</property>
            <property name="position">0</property>
          </packing>
        </child>
      </object>
    </child>
    <action-widgets>
      <action-widget response="-6">button1</action-widget>
      <action-widget response="-5">button2</action-widget>
    </action-widgets>
  </object>
  <object class="GtkSizeGroup" id="sizegroup1">
    <widgets>
      <widget name="label42"/>
      <widget name="label16"/>
      <widget name="label19"/>
      <widget name="label20"/>
      <widget name="label21"/>
      <widget name="label22"/>
      <widget name="label5"/>
      <widget name="lbl_samples"/>
      <widget name="lbl_roms"/>
      <widget name="lbl_mame_execs"/>
    </widgets>
  </object>
</interface>
