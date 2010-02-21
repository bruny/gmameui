<?xml version="1.0"?>
<interface>
  <requires lib="gtk+" version="2.16"/>
  <!-- interface-naming-policy toplevel-contextual -->
  <object class="GtkAdjustment" id="adj_heightscale">
    <property name="value">1</property>
    <property name="lower">1</property>
    <property name="upper">8</property>
    <property name="step_increment">1</property>
    <property name="page_increment">10</property>
    <property name="page_size">10</property>
  </object>
  <object class="GtkAdjustment" id="adj_beamsize">
    <property name="value">1</property>
    <property name="lower">1</property>
    <property name="upper">16</property>
    <property name="step_increment">1</property>
    <property name="page_increment">1</property>
    <property name="page_size">1</property>
  </object>
  <object class="GtkAdjustment" id="adj_gltexturesize">
    <property name="upper">2048</property>
    <property name="step_increment">1</property>
    <property name="page_increment">10</property>
    <property name="page_size">10</property>
  </object>
  <object class="GtkAdjustment" id="adj_openglbeamsize">
    <property name="upper">100</property>
    <property name="step_increment">1</property>
    <property name="page_increment">10</property>
    <property name="page_size">10</property>
  </object>
  <object class="GtkAdjustment" id="adj_buffersize">
    <property name="value">3.2999999999999998</property>
    <property name="lower">1</property>
    <property name="upper">30</property>
    <property name="step_increment">1</property>
    <property name="page_increment">1</property>
  </object>
  <object class="GtkAdjustment" id="adj_volume">
    <property name="lower">-32</property>
    <property name="step_increment">1</property>
    <property name="page_increment">1</property>
  </object>
  <object class="GtkAdjustment" id="adj_widthscale">
    <property name="value">1</property>
    <property name="lower">1</property>
    <property name="upper">8</property>
    <property name="step_increment">1</property>
    <property name="page_increment">10</property>
    <property name="page_size">10</property>
  </object>
  <object class="GtkAdjustment" id="adj_scalextoy">
    <property name="value">1</property>
    <property name="upper">4096</property>
    <property name="step_increment">1</property>
    <property name="page_increment">8</property>
    <property name="page_size">1</property>
  </object>
  <object class="GtkAdjustment" id="adj_gamma">
    <property name="value">0.5</property>
    <property name="lower">0.5</property>
    <property name="upper">2</property>
    <property name="step_increment">0.050000000000000003</property>
    <property name="page_increment">0.050000000000000003</property>
  </object>
  <object class="GtkAdjustment" id="adj_brightness">
    <property name="value">0.5</property>
    <property name="lower">0.5</property>
    <property name="upper">2</property>
    <property name="step_increment">0.050000000000000003</property>
    <property name="page_increment">0.050000000000000003</property>
  </object>
  <object class="GtkAdjustment" id="adj_pausebrightness">
    <property name="value">0.5</property>
    <property name="lower">0.5</property>
    <property name="upper">2</property>
    <property name="step_increment">0.050000000000000003</property>
    <property name="page_increment">0.050000000000000003</property>
  </object>
  <object class="GtkAdjustment" id="adj_maxframeskip">
    <property name="value">1</property>
    <property name="lower">1</property>
    <property name="upper">12</property>
    <property name="step_increment">1</property>
    <property name="page_increment">1</property>
    <property name="page_size">1</property>
  </object>
  <object class="GtkAdjustment" id="adj_intensity">
    <property name="value">0.5</property>
    <property name="lower">0.5</property>
    <property name="upper">3</property>
    <property name="step_increment">0.10000000000000001</property>
    <property name="page_increment">1</property>
  </object>
  <object class="GtkAdjustment" id="adj_flicker">
    <property name="upper">100</property>
    <property name="step_increment">1</property>
    <property name="page_increment">10</property>
    <property name="page_size">10</property>
  </object>
  <object class="GtkListStore" id="model_bpp">
    <columns>
      <!-- column-name gchararray -->
      <column type="gchararray"/>
    </columns>
    <data>
      <row>
        <col id="0" translatable="yes">Auto</col>
      </row>
      <row>
        <col id="0" translatable="yes">8 bits</col>
      </row>
      <row>
        <col id="0" translatable="yes">15 bits</col>
      </row>
      <row>
        <col id="0" translatable="yes">16 bits</col>
      </row>
      <row>
        <col id="0" translatable="yes">32 bits</col>
      </row>
    </data>
  </object>
  <object class="GtkListStore" id="model_mixerplugin">
    <columns>
      <!-- column-name gchararray -->
      <column type="gchararray"/>
    </columns>
    <data>
      <row>
        <col id="0" translatable="yes">OSS</col>
      </row>
    </data>
  </object>
  <object class="GtkListStore" id="model_dspplugin">
    <columns>
      <!-- column-name gchararray -->
      <column type="gchararray"/>
    </columns>
    <data>
      <row>
        <col id="0" translatable="yes">OSS</col>
      </row>
      <row>
        <col id="0" translatable="yes">SDL</col>
      </row>
      <row>
        <col id="0" translatable="yes">aRTS</col>
      </row>
    </data>
  </object>
  <object class="GtkListStore" id="model_joysticktype">
    <columns>
      <!-- column-name gchararray -->
      <column type="gchararray"/>
    </columns>
    <data>
      <row>
        <col id="0" translatable="yes">No joystick</col>
      </row>
      <row>
        <col id="0" translatable="yes">SDL joystick</col>
      </row>
    </data>
  </object>
  <object class="GtkListStore" id="model_effect">
    <columns>
      <!-- column-name gchararray -->
      <column type="gchararray"/>
    </columns>
    <data>
      <row>
        <col id="0" translatable="yes">None</col>
      </row>
      <row>
        <col id="0" translatable="yes">Smooth Scaling Effect</col>
      </row>
      <row>
        <col id="0" translatable="yes">Low Quality Filter</col>
      </row>
      <row>
        <col id="0" translatable="yes">High Quality Filter</col>
      </row>
      <row>
        <col id="0" translatable="yes">6-tap Filter with H-Scanlines</col>
      </row>
      <row>
        <col id="0" translatable="yes">Light Scanlines</col>
      </row>
      <row>
        <col id="0" translatable="yes">RGB Scanlines</col>
      </row>
      <row>
        <col id="0" translatable="yes">Deluxe Scanlines</col>
      </row>
      <row>
        <col id="0" translatable="yes">Black Scanlines</col>
      </row>
    </data>
  </object>
  <object class="GtkListStore" id="model_frameskip">
    <columns>
      <!-- column-name gchararray -->
      <column type="gchararray"/>
    </columns>
    <data>
      <row>
        <col id="0" translatable="yes">Draw every frame</col>
      </row>
      <row>
        <col id="0" translatable="yes">Skip 1 out of 12 frames</col>
      </row>
      <row>
        <col id="0" translatable="yes">Skip 2 out of 12 frames</col>
      </row>
      <row>
        <col id="0" translatable="yes">Skip 3 out of 12 frames</col>
      </row>
      <row>
        <col id="0" translatable="yes">Skip 4 out of 12 frames</col>
      </row>
      <row>
        <col id="0" translatable="yes">Skip 5 out of 12 frames</col>
      </row>
      <row>
        <col id="0" translatable="yes">Skip 6 out of 12 frames</col>
      </row>
      <row>
        <col id="0" translatable="yes">Skip 7 out of 12 frames</col>
      </row>
      <row>
        <col id="0" translatable="yes">Skip 8 out of 12 frames</col>
      </row>
      <row>
        <col id="0" translatable="yes">Skip 9 out of 12 frames</col>
      </row>
      <row>
        <col id="0" translatable="yes">Skip 10 out of 12 frames</col>
      </row>
      <row>
        <col id="0" translatable="yes">Skip 11 out of 12 frames</col>
      </row>
      <row>
        <col id="0" translatable="yes">Skip 12 out of 12 frames</col>
      </row>
    </data>
  </object>
  <object class="GtkListStore" id="model_vectorres">
    <columns>
      <!-- column-name gchararray -->
      <column type="gchararray"/>
    </columns>
    <data>
      <row>
        <col id="0" translatable="yes">320x200</col>
      </row>
      <row>
        <col id="0" translatable="yes">640x480</col>
      </row>
      <row>
        <col id="0" translatable="yes">800x600</col>
      </row>
      <row>
        <col id="0" translatable="yes">1024x768</col>
      </row>
      <row>
        <col id="0" translatable="yes">1280x1024</col>
      </row>
      <row>
        <col id="0" translatable="yes">1600x1200</col>
      </row>
    </data>
  </object>
  <object class="GtkListStore" id="model_artworkres">
    <columns>
      <!-- column-name gchararray -->
      <column type="gchararray"/>
    </columns>
    <data>
      <row>
        <col id="0" translatable="yes">Auto</col>
      </row>
      <row>
        <col id="0" translatable="yes">Standard</col>
      </row>
      <row>
        <col id="0" translatable="yes">High</col>
      </row>
    </data>
  </object>
  <object class="GtkListStore" id="model_neogeobios">
    <columns>
      <!-- column-name gchararray -->
      <column type="gchararray"/>
    </columns>
    <data>
      <row>
        <col id="0" translatable="yes">Europe, 1 Slot (also been seen on a 4 slot)</col>
      </row>
      <row>
        <col id="0" translatable="yes">Europe, 4 Slot</col>
      </row>
      <row>
        <col id="0" translatable="yes">US, 2 Slot</col>
      </row>
      <row>
        <col id="0" translatable="yes">US, 6 Slot (V5?)</col>
      </row>
      <row>
        <col id="0" translatable="yes">Asia S3 Ver 6</col>
      </row>
      <row>
        <col id="0" translatable="yes">Japan, Ver 6 VS Bios</col>
      </row>
      <row>
        <col id="0" translatable="yes">Japan, Older</col>
      </row>
      <row>
        <col id="0" translatable="yes">Universe Bios v1.0 (hack)</col>
      </row>
      <row>
        <col id="0" translatable="yes">Universe Bios v1.1 (hack)</col>
      </row>
      <row>
        <col id="0" translatable="yes">Debug (Development) Bios</col>
      </row>
      <row>
        <col id="0" translatable="yes">AES Console (Asia?) Bios</col>
      </row>
      <row>
        <col id="0" translatable="yes">Universe Bios v1.2 (hack)</col>
      </row>
    </data>
  </object>
  <object class="GtkListStore" id="model_samplerate">
    <columns>
      <!-- column-name gchararray -->
      <column type="gchararray"/>
    </columns>
    <data>
      <row>
        <col id="0" translatable="yes">8000</col>
      </row>
      <row>
        <col id="0" translatable="yes">11025</col>
      </row>
      <row>
        <col id="0" translatable="yes">16000</col>
      </row>
      <row>
        <col id="0" translatable="yes">22050</col>
      </row>
      <row>
        <col id="0" translatable="yes">44100</col>
      </row>
      <row>
        <col id="0" translatable="yes">48000</col>
      </row>
    </data>
  </object>
  <object class="GtkListStore" id="model_audiodevice">
    <columns>
      <!-- column-name gchararray -->
      <column type="gchararray"/>
    </columns>
    <data>
      <row>
        <col id="0" translatable="yes">/dev/dsp</col>
      </row>
      <row>
        <col id="0" translatable="yes">/dev/dsp0</col>
      </row>
      <row>
        <col id="0" translatable="yes">/dev/dsp1</col>
      </row>
      <row>
        <col id="0" translatable="yes">/dev/dsp2</col>
      </row>
      <row>
        <col id="0" translatable="yes">/dev/dsp3</col>
      </row>
      <row>
        <col id="0" translatable="yes">/dev/audio</col>
      </row>
      <row>
        <col id="0" translatable="yes">/dev/null</col>
      </row>
    </data>
  </object>
  <object class="GtkListStore" id="model_mixerdevice">
    <columns>
      <!-- column-name gchararray -->
      <column type="gchararray"/>
    </columns>
    <data>
      <row>
        <col id="0" translatable="yes">/dev/mixer</col>
      </row>
    </data>
  </object>
  <object class="GtkWindow" id="Display">
    <child>
      <object class="GtkNotebook" id="DisplayNotebook">
        <property name="visible">True</property>
        <property name="can_focus">True</property>
        <child>
          <object class="GtkVBox" id="vbox10">
            <property name="visible">True</property>
            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
            <property name="spacing">6</property>
            <child>
              <object class="GtkLabel" id="label29">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="xalign">0</property>
                <property name="label" translatable="yes">&lt;b&gt;Display options&lt;/b&gt;</property>
                <property name="use_markup">True</property>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="position">0</property>
              </packing>
            </child>
            <child>
              <object class="GtkAlignment" id="alignment8">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="left_padding">12</property>
                <child>
                  <object class="GtkVBox" id="vbox12">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="spacing">6</property>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Video-fullscreen">
                        <property name="label" translatable="yes">Fullscreen</property>
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
                      <object class="GtkCheckButton" id="preferences_Video-dirty">
                        <property name="label" translatable="yes">Draw only changes</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">1</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Video-throttle">
                        <property name="label" translatable="yes">Throttle</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">2</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Video-sleepidle">
                        <property name="label" translatable="yes">Sleep when idle</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">3</property>
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
            <child>
              <object class="GtkLabel" id="label31">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="xalign">0</property>
                <property name="label" translatable="yes">&lt;b&gt;Resolution&lt;/b&gt;</property>
                <property name="use_markup">True</property>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="position">2</property>
              </packing>
            </child>
            <child>
              <object class="GtkAlignment" id="alignment10">
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
                      <object class="GtkCheckButton" id="preferences_Video-autodouble">
                        <property name="label" translatable="yes">Autodouble</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="right_attach">2</property>
                        <property name="top_attach">4</property>
                        <property name="bottom_attach">5</property>
                        <property name="x_options">GTK_FILL</property>
                        <property name="y_options">GTK_FILL</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkLabel" id="heightscale_label">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Height scale:</property>
                      </object>
                      <packing>
                        <property name="x_options">GTK_FILL</property>
                        <property name="y_options">GTK_FILL</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkLabel" id="widthscale_label">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Width scale:</property>
                      </object>
                      <packing>
                        <property name="top_attach">1</property>
                        <property name="bottom_attach">2</property>
                        <property name="x_options">GTK_FILL</property>
                        <property name="y_options">GTK_FILL</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkLabel" id="bpp_label">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Bit per pixels:</property>
                      </object>
                      <packing>
                        <property name="top_attach">2</property>
                        <property name="bottom_attach">3</property>
                        <property name="x_options">GTK_FILL</property>
                        <property name="y_options">GTK_FILL</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkComboBox" id="preferences_Video-bpp">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="model">model_bpp</property>
                        <child>
                          <object class="GtkCellRendererText" id="renderer1"/>
                          <attributes>
                            <attribute name="text">0</attribute>
                          </attributes>
                        </child>
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
                      <object class="GtkSpinButton" id="preferences_Video-heightscale">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="max_length">4</property>
                        <property name="adjustment">adj_heightscale</property>
                        <property name="climb_rate">1</property>
                        <property name="numeric">True</property>
                      </object>
                      <packing>
                        <property name="left_attach">1</property>
                        <property name="right_attach">2</property>
                        <property name="x_options">GTK_FILL</property>
                        <property name="y_options">GTK_FILL</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkSpinButton" id="preferences_Video-widthscale">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="max_length">4</property>
                        <property name="adjustment">adj_widthscale</property>
                        <property name="climb_rate">1</property>
                        <property name="numeric">True</property>
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
                    <child>
                      <object class="GtkSpinButton" id="preferences_Video-arbheight">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="max_length">4</property>
                        <property name="adjustment">adj_scalextoy</property>
                        <property name="climb_rate">1</property>
                        <property name="numeric">True</property>
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
                      <object class="GtkLabel" id="label41">
                        <property name="visible">True</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Scale video to height:</property>
                      </object>
                      <packing>
                        <property name="top_attach">3</property>
                        <property name="bottom_attach">4</property>
                        <property name="x_options">GTK_FILL</property>
                      </packing>
                    </child>
                  </object>
                </child>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="position">3</property>
              </packing>
            </child>
            <child>
              <object class="GtkLabel" id="label32">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="xalign">0</property>
                <property name="label" translatable="yes">&lt;b&gt;Rotation&lt;/b&gt;</property>
                <property name="use_markup">True</property>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="position">4</property>
              </packing>
            </child>
            <child>
              <object class="GtkAlignment" id="alignment6">
                <property name="visible">True</property>
                <property name="left_padding">12</property>
                <child>
                  <object class="GtkVBox" id="vbox18">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="spacing">6</property>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Video-ror">
                        <property name="label" translatable="yes">Rotate right</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Video-rol">
                        <property name="label" translatable="yes">Rotate left</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">1</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Video-norotate">
                        <property name="label" translatable="yes">Do not apply rotation</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">2</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Video-flipy">
                        <property name="label" translatable="yes">Flip screen upside-down</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">3</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Video-flipx">
                        <property name="label" translatable="yes">Flip screen left-right</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">4</property>
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
          </object>
        </child>
        <child type="tab">
          <object class="GtkLabel" id="label1">
            <property name="visible">True</property>
            <property name="label" translatable="yes">Display Options</property>
          </object>
          <packing>
            <property name="tab_fill">False</property>
          </packing>
        </child>
        <child>
          <object class="GtkVBox" id="vbox11">
            <property name="visible">True</property>
            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
            <property name="spacing">6</property>
            <child>
              <object class="GtkLabel" id="label34">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="xalign">0</property>
                <property name="label" translatable="yes">&lt;b&gt;Effects&lt;/b&gt;</property>
                <property name="use_markup">True</property>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="position">0</property>
              </packing>
            </child>
            <child>
              <object class="GtkAlignment" id="alignment12">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="left_padding">12</property>
                <child>
                  <object class="GtkVBox" id="effects_vbox">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="spacing">6</property>
                    <child>
                      <object class="GtkComboBox" id="preferences_Video-effect">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="model">model_effect</property>
                        <child>
                          <object class="GtkCellRendererText" id="renderer2"/>
                          <attributes>
                            <attribute name="text">0</attribute>
                          </attributes>
                        </child>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Video-scanlines">
                        <property name="label" translatable="yes">Use Scanlines</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">1</property>
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
            <child>
              <object class="GtkLabel" id="label36">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="xalign">0</property>
                <property name="label" translatable="yes">&lt;b&gt;Corrections&lt;/b&gt;</property>
                <property name="use_markup">True</property>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="position">2</property>
              </packing>
            </child>
            <child>
              <object class="GtkAlignment" id="alignment13">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="left_padding">12</property>
                <child>
                  <object class="GtkVBox" id="correction_vbox">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <child>
                      <object class="GtkLabel" id="gamma_label">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Gamma correction</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkHScale" id="preferences_Video-gamma">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="adjustment">adj_gamma</property>
                        <property name="digits">2</property>
                        <property name="value_pos">right</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">1</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkLabel" id="brightness_label">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Brightness correction</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">2</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkHScale" id="preferences_Video-brightness">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="adjustment">adj_brightness</property>
                        <property name="digits">2</property>
                        <property name="value_pos">right</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">3</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkLabel" id="brightness_label1">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Pause brightness</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">4</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkHScale" id="preferences_Video-pause-brightness">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="adjustment">adj_pausebrightness</property>
                        <property name="digits">2</property>
                        <property name="value_pos">right</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">5</property>
                      </packing>
                    </child>
                  </object>
                </child>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="position">3</property>
              </packing>
            </child>
            <child>
              <object class="GtkLabel" id="label37">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="xalign">0</property>
                <property name="label" translatable="yes">&lt;b&gt;Frame skipping&lt;/b&gt;</property>
                <property name="use_markup">True</property>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="position">4</property>
              </packing>
            </child>
            <child>
              <object class="GtkAlignment" id="alignment14">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="left_padding">12</property>
                <child>
                  <object class="GtkTable" id="table3">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="n_rows">3</property>
                    <property name="n_columns">2</property>
                    <property name="column_spacing">6</property>
                    <property name="row_spacing">6</property>
                    <child>
                      <object class="GtkComboBox" id="preferences_Video-frameskip">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="model">model_frameskip</property>
                        <child>
                          <object class="GtkCellRendererText" id="renderer3"/>
                          <attributes>
                            <attribute name="text">0</attribute>
                          </attributes>
                        </child>
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
                    <child>
                      <object class="GtkLabel" id="fs_label">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Frames to skip:</property>
                      </object>
                      <packing>
                        <property name="top_attach">1</property>
                        <property name="bottom_attach">2</property>
                        <property name="x_options">GTK_FILL</property>
                        <property name="y_options">GTK_FILL</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Video-autoframeskip">
                        <property name="label" translatable="yes">Automatic</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="right_attach">2</property>
                        <property name="x_options">GTK_FILL</property>
                        <property name="y_options">GTK_FILL</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkLabel" id="maxfs_label">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Max frameskip:</property>
                      </object>
                      <packing>
                        <property name="top_attach">2</property>
                        <property name="bottom_attach">3</property>
                        <property name="x_options">GTK_FILL</property>
                        <property name="y_options">GTK_FILL</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkSpinButton" id="preferences_Video-maxautoframeskip">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="adjustment">adj_maxframeskip</property>
                      </object>
                      <packing>
                        <property name="left_attach">1</property>
                        <property name="right_attach">2</property>
                        <property name="top_attach">2</property>
                        <property name="bottom_attach">3</property>
                        <property name="x_options">GTK_FILL</property>
                      </packing>
                    </child>
                  </object>
                </child>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="position">5</property>
              </packing>
            </child>
          </object>
          <packing>
            <property name="position">1</property>
          </packing>
        </child>
        <child type="tab">
          <object class="GtkLabel" id="label3">
            <property name="visible">True</property>
            <property name="label" translatable="yes">Effects &amp; Corrections</property>
          </object>
          <packing>
            <property name="position">1</property>
            <property name="tab_fill">False</property>
          </packing>
        </child>
        <child>
          <object class="GtkVBox" id="vector_vbox">
            <property name="visible">True</property>
            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
            <child>
              <object class="GtkLabel" id="label2">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="xalign">0</property>
                <property name="label" translatable="yes">&lt;b&gt;Vector options&lt;/b&gt;</property>
                <property name="use_markup">True</property>
              </object>
              <packing>
                <property name="expand">False</property>
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
                  <object class="GtkVBox" id="vbox4">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="spacing">6</property>
                    <child>
                      <object class="GtkHBox" id="hbox1">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="spacing">6</property>
                        <child>
                          <object class="GtkLabel" id="label21">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Vector resolution:</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkComboBox" id="preferences_Vector-vectorres">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="model">model_vectorres</property>
                            <child>
                              <object class="GtkCellRendererText" id="renderer4"/>
                              <attributes>
                                <attribute name="text">0</attribute>
                              </attributes>
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
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkTable" id="table4">
                        <property name="visible">True</property>
                        <property name="n_rows">3</property>
                        <property name="n_columns">2</property>
                        <property name="column_spacing">6</property>
                        <property name="row_spacing">6</property>
                        <child>
                          <object class="GtkHScale" id="preferences_Vector-intensity">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="adjustment">adj_intensity</property>
                            <property name="digits">2</property>
                            <property name="value_pos">right</property>
                          </object>
                          <packing>
                            <property name="left_attach">1</property>
                            <property name="right_attach">2</property>
                            <property name="top_attach">2</property>
                            <property name="bottom_attach">3</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkHScale" id="preferences_Vector-flicker">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="adjustment">adj_flicker</property>
                            <property name="digits">0</property>
                            <property name="value_pos">right</property>
                          </object>
                          <packing>
                            <property name="left_attach">1</property>
                            <property name="right_attach">2</property>
                            <property name="top_attach">1</property>
                            <property name="bottom_attach">2</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkHScale" id="preferences_Vector-beam">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="adjustment">adj_beamsize</property>
                            <property name="digits">2</property>
                            <property name="value_pos">right</property>
                          </object>
                          <packing>
                            <property name="left_attach">1</property>
                            <property name="right_attach">2</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="intensity_label">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Intensity</property>
                          </object>
                          <packing>
                            <property name="top_attach">2</property>
                            <property name="bottom_attach">3</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options">GTK_FILL</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="flicker_label">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Flicker</property>
                          </object>
                          <packing>
                            <property name="top_attach">1</property>
                            <property name="bottom_attach">2</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options">GTK_FILL</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="beam_label">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Beam size</property>
                          </object>
                          <packing>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options">GTK_FILL</property>
                          </packing>
                        </child>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="padding">6</property>
                        <property name="position">1</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Vector-antialias">
                        <property name="label" translatable="yes">Draw antialiased vectors</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">2</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Vector-translucency">
                        <property name="label" translatable="yes">Draw translucent vectors</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">3</property>
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
            <property name="position">2</property>
          </packing>
        </child>
        <child type="tab">
          <object class="GtkLabel" id="label5">
            <property name="visible">True</property>
            <property name="label" translatable="yes">Vector</property>
          </object>
          <packing>
            <property name="position">2</property>
            <property name="tab_fill">False</property>
          </packing>
        </child>
        <child>
          <object class="GtkVBox" id="vbox1">
            <property name="visible">True</property>
            <child>
              <object class="GtkLabel" id="label8">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="xalign">0</property>
                <property name="label" translatable="yes">&lt;b&gt;Artwork options&lt;/b&gt;</property>
                <property name="use_markup">True</property>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="padding">6</property>
                <property name="position">0</property>
              </packing>
            </child>
            <child>
              <object class="GtkAlignment" id="alignment1">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="left_padding">12</property>
                <child>
                  <object class="GtkVBox" id="artwork_table1">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="spacing">6</property>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Artwork-artwork">
                        <property name="label" translatable="yes">Use additional game artwork:</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkAlignment" id="alignment24">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="left_padding">12</property>
                        <child>
                          <object class="GtkVBox" id="vbox6">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="spacing">6</property>
                            <child>
                              <object class="GtkCheckButton" id="preferences_Artwork-use_backdrops">
                                <property name="label" translatable="yes">Backdrops</property>
                                <property name="visible">True</property>
                                <property name="can_focus">True</property>
                                <property name="receives_default">False</property>
                                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                <property name="draw_indicator">True</property>
                              </object>
                              <packing>
                                <property name="expand">False</property>
                                <property name="position">0</property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkCheckButton" id="preferences_Artwork-use_bezels">
                                <property name="label" translatable="yes">Bezels</property>
                                <property name="visible">True</property>
                                <property name="can_focus">True</property>
                                <property name="receives_default">False</property>
                                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                <property name="draw_indicator">True</property>
                              </object>
                              <packing>
                                <property name="expand">False</property>
                                <property name="position">1</property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkCheckButton" id="preferences_Artwork-use_overlays">
                                <property name="label" translatable="yes">Overlays</property>
                                <property name="visible">True</property>
                                <property name="can_focus">True</property>
                                <property name="receives_default">False</property>
                                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                                <property name="draw_indicator">True</property>
                              </object>
                              <packing>
                                <property name="expand">False</property>
                                <property name="position">2</property>
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
                    <child>
                      <object class="GtkHBox" id="hbox14">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="spacing">6</property>
                        <child>
                          <object class="GtkLabel" id="label7">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="label" translatable="yes">Artwork resolution:</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkComboBox" id="preferences_Artwork-artwork_resolution">
                            <property name="visible">True</property>
                            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                            <property name="model">model_artworkres</property>
                            <child>
                              <object class="GtkCellRendererText" id="renderer5"/>
                              <attributes>
                                <attribute name="text">0</attribute>
                              </attributes>
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
                        <property name="position">2</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Artwork-artwork_crop">
                        <property name="label" translatable="yes">Crop artwork</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">3</property>
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
            <property name="position">3</property>
          </packing>
        </child>
        <child type="tab">
          <object class="GtkLabel" id="label6">
            <property name="visible">True</property>
            <property name="label" translatable="yes">Artwork</property>
          </object>
          <packing>
            <property name="position">3</property>
            <property name="tab_fill">False</property>
          </packing>
        </child>
        <child>
          <object class="GtkVBox" id="vbox7">
            <property name="visible">True</property>
            <property name="spacing">6</property>
            <child>
              <object class="GtkLabel" id="label28">
                <property name="visible">True</property>
                <property name="xalign">0</property>
                <property name="label" translatable="yes">&lt;b&gt;OpenGL&lt;/b&gt;</property>
                <property name="use_markup">True</property>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="position">0</property>
              </packing>
            </child>
            <child>
              <object class="GtkAlignment" id="alignment16">
                <property name="visible">True</property>
                <property name="left_padding">12</property>
                <child>
                  <object class="GtkVBox" id="vbox23">
                    <property name="visible">True</property>
                    <property name="spacing">6</property>
                    <child>
                      <object class="GtkCheckButton" id="preferences_OpenGL-gldblbuffer">
                        <property name="label" translatable="yes">Enable double buffering</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkHBox" id="hbox2">
                        <property name="visible">True</property>
                        <property name="spacing">6</property>
                        <child>
                          <object class="GtkLabel" id="label16">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">GL Texture size</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkSpinButton" id="preferences_OpenGL-gltexture_size">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="adjustment">adj_gltexturesize</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="position">1</property>
                          </packing>
                        </child>
                      </object>
                      <packing>
                        <property name="position">1</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_OpenGL-glbilinear">
                        <property name="label" translatable="yes">Enable bilinear filtering</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">2</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkHBox" id="hbox19">
                        <property name="visible">True</property>
                        <child>
                          <object class="GtkLabel" id="label30">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Beam size for vector games</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkHScale" id="preferences_OpenGL-glbeam">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="adjustment">adj_openglbeamsize</property>
                            <property name="value_pos">right</property>
                          </object>
                          <packing>
                            <property name="position">1</property>
                          </packing>
                        </child>
                      </object>
                      <packing>
                        <property name="position">3</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_OpenGL-glantialias">
                        <property name="label" translatable="yes">Enable antialiasing</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">4</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_OpenGL-glantialiasvec">
                        <property name="label" translatable="yes">Enable vector antialiasing</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">5</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_OpenGL-cabview">
                        <property name="label" translatable="yes">Start in cabinet view</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">6</property>
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
            <property name="position">4</property>
          </packing>
        </child>
        <child type="tab">
          <object class="GtkLabel" id="label27">
            <property name="visible">True</property>
            <property name="label" translatable="yes">OpenGL</property>
          </object>
          <packing>
            <property name="position">4</property>
            <property name="tab_fill">False</property>
          </packing>
        </child>
      </object>
    </child>
  </object>
  <object class="GtkWindow" id="Miscellaneous">
    <child>
      <object class="GtkVBox" id="MiscVBox">
        <property name="visible">True</property>
        <property name="spacing">6</property>
        <child>
          <object class="GtkLabel" id="label10">
            <property name="visible">True</property>
            <property name="xalign">0</property>
            <property name="label" translatable="yes">&lt;b&gt;Miscellaneous Options&lt;/b&gt;</property>
            <property name="use_markup">True</property>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="position">0</property>
          </packing>
        </child>
        <child>
          <object class="GtkAlignment" id="alignment5">
            <property name="visible">True</property>
            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
            <property name="left_padding">12</property>
            <child>
              <object class="GtkVBox" id="vbox24">
                <property name="visible">True</property>
                <property name="spacing">6</property>
                <child>
                  <object class="GtkCheckButton" id="preferences_MAME-cheat">
                    <property name="label" translatable="yes">Enable game cheats</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="position">0</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_MAME-skip_disclaimer">
                    <property name="label" translatable="yes">Skip disclaimer info</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="position">1</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_MAME-skip_gameinfo">
                    <property name="label" translatable="yes">Skip game info</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="position">2</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkHBox" id="hbox4">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="spacing">6</property>
                    <child>
                      <object class="GtkLabel" id="bios_label">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Neo-Geo Bios:</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkComboBox" id="preferences_MAME-bios">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="model">model_neogeobios</property>
                        <child>
                          <object class="GtkCellRendererText" id="renderer6"/>
                          <attributes>
                            <attribute name="text">0</attribute>
                          </attributes>
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
                    <property name="position">3</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_MAME-autosave">
                    <property name="label" translatable="yes">Enable automatic save/restore</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="position">4</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_MAME-keyboard_leds">
                    <property name="label" translatable="yes">Use keyboard LEDs as game indicators</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="position">5</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkTable" id="table5">
                    <property name="visible">True</property>
                    <property name="n_rows">2</property>
                    <property name="n_columns">2</property>
                    <property name="column_spacing">6</property>
                    <property name="row_spacing">6</property>
                    <child>
                      <object class="GtkEntry" id="preferences_File.stderr-file">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                      </object>
                      <packing>
                        <property name="left_attach">1</property>
                        <property name="right_attach">2</property>
                        <property name="top_attach">1</property>
                        <property name="bottom_attach">2</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkEntry" id="preferences_File.stdout-file">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                      </object>
                      <packing>
                        <property name="left_attach">1</property>
                        <property name="right_attach">2</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkLabel" id="stderr_lbl">
                        <property name="visible">True</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Stderr logfile</property>
                      </object>
                      <packing>
                        <property name="top_attach">1</property>
                        <property name="bottom_attach">2</property>
                        <property name="x_options">GTK_FILL</property>
                        <property name="y_options">GTK_FILL</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkLabel" id="stdout_lbl">
                        <property name="visible">True</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Stdout logfile</property>
                      </object>
                      <packing>
                        <property name="x_options">GTK_FILL</property>
                        <property name="y_options">GTK_FILL</property>
                      </packing>
                    </child>
                  </object>
                  <packing>
                    <property name="position">6</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_File-log">
                    <property name="label" translatable="yes">Log debug info</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="position">7</property>
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
    </child>
  </object>
  <object class="GtkWindow" id="Sound">
    <child>
      <object class="GtkVBox" id="SoundVBox">
        <property name="visible">True</property>
        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
        <property name="spacing">6</property>
        <child>
          <object class="GtkLabel" id="label11">
            <property name="visible">True</property>
            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
            <property name="xalign">0</property>
            <property name="label" translatable="yes">&lt;b&gt;General sound options&lt;/b&gt;</property>
            <property name="use_markup">True</property>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="position">0</property>
          </packing>
        </child>
        <child>
          <object class="GtkAlignment" id="alignment11">
            <property name="visible">True</property>
            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
            <property name="left_padding">12</property>
            <child>
              <object class="GtkTable" id="table11">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="n_rows">8</property>
                <property name="n_columns">2</property>
                <property name="column_spacing">6</property>
                <property name="row_spacing">6</property>
                <child>
                  <object class="GtkHBox" id="bufsize_hbox1">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="spacing">6</property>
                    <child>
                      <object class="GtkLabel" id="label14">
                        <property name="visible">True</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Buffer size</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkHScale" id="preferences_Sound-bufsize">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="adjustment">adj_buffersize</property>
                        <property name="value_pos">right</property>
                      </object>
                      <packing>
                        <property name="position">1</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkLabel" id="ms_label1">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="label" translatable="yes">frames</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">2</property>
                      </packing>
                    </child>
                  </object>
                  <packing>
                    <property name="right_attach">2</property>
                    <property name="top_attach">4</property>
                    <property name="bottom_attach">5</property>
                    <property name="x_options">GTK_FILL</property>
                    <property name="y_options">GTK_FILL</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkHBox" id="volume_hbox1">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="spacing">6</property>
                    <child>
                      <object class="GtkLabel" id="label12">
                        <property name="visible">True</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Volume</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkHScale" id="preferences_Sound-volume">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="adjustment">adj_volume</property>
                        <property name="digits">0</property>
                        <property name="value_pos">right</property>
                      </object>
                      <packing>
                        <property name="position">1</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkLabel" id="db_label1">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="label" translatable="yes">dB</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">2</property>
                      </packing>
                    </child>
                  </object>
                  <packing>
                    <property name="right_attach">2</property>
                    <property name="top_attach">3</property>
                    <property name="bottom_attach">4</property>
                    <property name="x_options">GTK_FILL</property>
                    <property name="y_options">GTK_FILL</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkComboBox" id="preferences_Sound-samplefreq">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="model">model_samplerate</property>
                    <child>
                      <object class="GtkCellRendererText" id="renderer7"/>
                      <attributes>
                        <attribute name="text">0</attribute>
                      </attributes>
                    </child>
                  </object>
                  <packing>
                    <property name="left_attach">1</property>
                    <property name="right_attach">2</property>
                    <property name="top_attach">5</property>
                    <property name="bottom_attach">6</property>
                    <property name="x_options">GTK_FILL</property>
                    <property name="y_options">GTK_FILL</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkComboBoxEntry" id="Preferences_Sound-audiodevice">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="model">model_audiodevice</property>
                    <child>
                      <object class="GtkCellRendererText" id="renderer8"/>
                      <attributes>
                        <attribute name="text">0</attribute>
                      </attributes>
                    </child>
                  </object>
                  <packing>
                    <property name="left_attach">1</property>
                    <property name="right_attach">2</property>
                    <property name="top_attach">6</property>
                    <property name="bottom_attach">7</property>
                    <property name="x_options">GTK_FILL</property>
                    <property name="y_options">GTK_FILL</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkComboBoxEntry" id="Preferences_Sound-mixerdevice">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="model">model_mixerdevice</property>
                    <child>
                      <object class="GtkCellRendererText" id="renderer9"/>
                      <attributes>
                        <attribute name="text">0</attribute>
                      </attributes>
                    </child>
                  </object>
                  <packing>
                    <property name="left_attach">1</property>
                    <property name="right_attach">2</property>
                    <property name="top_attach">7</property>
                    <property name="bottom_attach">8</property>
                    <property name="x_options">GTK_FILL</property>
                    <property name="y_options">GTK_FILL</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkLabel" id="samplefreq_label1">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="xalign">0</property>
                    <property name="label" translatable="yes">Sample rate:</property>
                  </object>
                  <packing>
                    <property name="top_attach">5</property>
                    <property name="bottom_attach">6</property>
                    <property name="x_options">GTK_FILL</property>
                    <property name="y_options">GTK_FILL</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkLabel" id="audiodevice_label1">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="xalign">0</property>
                    <property name="label" translatable="yes">Audio device:</property>
                  </object>
                  <packing>
                    <property name="top_attach">6</property>
                    <property name="bottom_attach">7</property>
                    <property name="x_options">GTK_FILL</property>
                    <property name="y_options">GTK_FILL</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkLabel" id="mixerdevice_label1">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="xalign">0</property>
                    <property name="label" translatable="yes">Mixer device:</property>
                  </object>
                  <packing>
                    <property name="top_attach">7</property>
                    <property name="bottom_attach">8</property>
                    <property name="x_options">GTK_FILL</property>
                    <property name="y_options">GTK_FILL</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_Sound-fakesound">
                    <property name="label" translatable="yes">Fake sound</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="right_attach">2</property>
                    <property name="top_attach">2</property>
                    <property name="bottom_attach">3</property>
                    <property name="x_options">GTK_FILL</property>
                    <property name="y_options">GTK_FILL</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_Sound-samples">
                    <property name="label" translatable="yes">Use samples</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="right_attach">2</property>
                    <property name="top_attach">1</property>
                    <property name="bottom_attach">2</property>
                    <property name="x_options">GTK_FILL</property>
                    <property name="y_options">GTK_FILL</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_Sound-sound">
                    <property name="label" translatable="yes">Enable sound</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="right_attach">2</property>
                    <property name="x_options">GTK_FILL</property>
                    <property name="y_options">GTK_FILL</property>
                  </packing>
                </child>
              </object>
            </child>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="padding">6</property>
            <property name="position">1</property>
          </packing>
        </child>
        <child>
          <object class="GtkLabel" id="label17">
            <property name="visible">True</property>
            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
            <property name="xalign">0</property>
            <property name="label" translatable="yes">&lt;b&gt;Digital sound and mixer options&lt;/b&gt;</property>
            <property name="use_markup">True</property>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="position">2</property>
          </packing>
        </child>
        <child>
          <object class="GtkAlignment" id="alignment28">
            <property name="visible">True</property>
            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
            <property name="left_padding">12</property>
            <child>
              <object class="GtkTable" id="table12">
                <property name="visible">True</property>
                <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                <property name="n_rows">4</property>
                <property name="n_columns">2</property>
                <property name="column_spacing">6</property>
                <property name="row_spacing">6</property>
                <child>
                  <object class="GtkLabel" id="soundmixerlabel">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="xalign">0</property>
                    <property name="label" translatable="yes">Mixer plugin:</property>
                  </object>
                  <packing>
                    <property name="top_attach">1</property>
                    <property name="bottom_attach">2</property>
                    <property name="x_options">GTK_FILL</property>
                    <property name="y_options">GTK_FILL</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkLabel" id="soundfile_label1">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="xalign">0</property>
                    <property name="label" translatable="yes">Sound file name:</property>
                  </object>
                  <packing>
                    <property name="top_attach">2</property>
                    <property name="bottom_attach">3</property>
                    <property name="x_options">GTK_FILL</property>
                    <property name="y_options">GTK_FILL</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkComboBox" id="preferences_Sound-sound_mixer_plugin">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="model">model_mixerplugin</property>
                    <child>
                      <object class="GtkCellRendererText" id="renderer10"/>
                      <attributes>
                        <attribute name="text">0</attribute>
                      </attributes>
                    </child>
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
                <child>
                  <object class="GtkEntry" id="soundfile_entry1">
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="max_length">50</property>
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
                  <object class="GtkLabel" id="dsp_plugin_label1">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="xalign">0</property>
                    <property name="label" translatable="yes">DSP Plugin:</property>
                  </object>
                  <packing>
                    <property name="x_options">GTK_FILL</property>
                    <property name="y_options">GTK_FILL</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkComboBox" id="preferences_Sound-dsp_plugin">
                    <property name="visible">True</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="model">model_dspplugin</property>
                    <child>
                      <object class="GtkCellRendererText" id="renderer11"/>
                      <attributes>
                        <attribute name="text">0</attribute>
                      </attributes>
                    </child>
                  </object>
                  <packing>
                    <property name="left_attach">1</property>
                    <property name="right_attach">2</property>
                    <property name="x_options">GTK_FILL</property>
                    <property name="y_options">GTK_FILL</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_Sound-timer">
                    <property name="label" translatable="yes">Timer based audio</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="right_attach">2</property>
                    <property name="top_attach">3</property>
                    <property name="bottom_attach">4</property>
                    <property name="x_options">GTK_FILL</property>
                    <property name="y_options">GTK_FILL</property>
                  </packing>
                </child>
              </object>
            </child>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="padding">6</property>
            <property name="position">3</property>
          </packing>
        </child>
      </object>
    </child>
  </object>
  <object class="GtkWindow" id="Input">
    <child>
      <object class="GtkVBox" id="InputVBox">
        <property name="visible">True</property>
        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
        <property name="spacing">6</property>
        <child>
          <object class="GtkLabel" id="label9">
            <property name="visible">True</property>
            <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
            <property name="xalign">0</property>
            <property name="label" translatable="yes">&lt;b&gt;Joystick&lt;/b&gt;</property>
            <property name="use_markup">True</property>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="position">0</property>
          </packing>
        </child>
        <child>
          <object class="GtkAlignment" id="alignment17">
            <property name="visible">True</property>
            <property name="left_padding">12</property>
            <child>
              <object class="GtkVBox" id="vbox3">
                <property name="visible">True</property>
                <property name="spacing">6</property>
                <child>
                  <object class="GtkHBox" id="hbox10">
                    <property name="visible">True</property>
                    <child>
                      <object class="GtkLabel" id="joytype_label1">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Joystick type:</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkComboBox" id="preferences_Input-joytype">
                        <property name="visible">True</property>
                        <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                        <property name="model">model_joysticktype</property>
                        <child>
                          <object class="GtkCellRendererText" id="renderer12"/>
                          <attributes>
                            <attribute name="text">0</attribute>
                          </attributes>
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
                    <property name="position">0</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_Input-analogstick">
                    <property name="label" translatable="yes">Analog joystick</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="position">1</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_Input-ugcicoin">
                    <property name="label" translatable="yes">UGCI (tm) Coin/Play support</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="position">2</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_Input-steadykey">
                    <property name="label" translatable="yes">Enable steadykey support</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="position">3</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_Input-usbpspad">
                    <property name="label" translatable="yes">USB PS Game Pads</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="position">4</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_Input-rapidfire">
                    <property name="label" translatable="yes">Rapid-fire</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="events">GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="position">5</property>
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
        <child>
          <object class="GtkLabel" id="label13">
            <property name="visible">True</property>
            <property name="xalign">0</property>
            <property name="label" translatable="yes">&lt;b&gt;X11-input&lt;/b&gt;</property>
            <property name="use_markup">True</property>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="position">2</property>
          </packing>
        </child>
        <child>
          <object class="GtkAlignment" id="alignment4">
            <property name="visible">True</property>
            <property name="left_padding">12</property>
            <child>
              <object class="GtkVBox" id="vbox2">
                <property name="visible">True</property>
                <property name="spacing">6</property>
                <child>
                  <object class="GtkCheckButton" id="preferences_X11Input-grabmouse">
                    <property name="label" translatable="yes">Select mouse grabbing</property>
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
                  <object class="GtkCheckButton" id="preferences_X11Input-grabkeyboard">
                    <property name="label" translatable="yes">Select keyboard grabbing</property>
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
                  <object class="GtkCheckButton" id="preferences_X11Input-alwaysusemouse">
                    <property name="label" translatable="yes">Always use mouse</property>
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
                  <object class="GtkCheckButton" id="preferences_X11Input-cursor">
                    <property name="label" translatable="yes">Show cursor</property>
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
                  <object class="GtkCheckButton" id="preferences_X11Input-winkeys">
                    <property name="label" translatable="yes">Enable mapping of Windows keys</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="position">4</property>
                  </packing>
                </child>
              </object>
            </child>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="position">3</property>
          </packing>
        </child>
      </object>
    </child>
  </object>
</interface>
