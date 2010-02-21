<?xml version="1.0"?>
<interface>
  <requires lib="gtk+" version="2.16"/>
  <!-- interface-naming-policy toplevel-contextual -->
  <object class="GtkAdjustment" id="adj_frameskip">
    <property name="upper">12</property>
    <property name="step_increment">1</property>
    <property name="page_increment">10</property>
    <property name="page_size">10</property>
  </object>
  <object class="GtkAdjustment" id="adj_flicker">
    <property name="upper">100</property>
    <property name="step_increment">1</property>
    <property name="page_increment">10</property>
    <property name="page_size">10</property>
  </object>
  <object class="GtkAdjustment" id="adj_prescale">
    <property name="value">1</property>
    <property name="lower">1</property>
    <property name="upper">8</property>
    <property name="step_increment">1</property>
    <property name="page_increment">10</property>
    <property name="page_size">10</property>
  </object>
  <object class="GtkAdjustment" id="adj_speed">
    <property name="value">1</property>
    <property name="lower">0.10000000000000001</property>
    <property name="upper">4</property>
    <property name="step_increment">0.10000000000000001</property>
    <property name="page_increment">1</property>
    <property name="page_size">1</property>
  </object>
  <object class="GtkAdjustment" id="adj_volume">
    <property name="value">-10</property>
    <property name="lower">-32</property>
    <property name="step_increment">1</property>
    <property name="page_increment">10</property>
    <property name="page_size">10</property>
  </object>
  <object class="GtkAdjustment" id="ad_audiolatency">
    <property name="value">3</property>
    <property name="upper">100</property>
    <property name="step_increment">1</property>
    <property name="page_increment">10</property>
    <property name="page_size">10</property>
  </object>
  <object class="GtkAdjustment" id="adj_brightness">
    <property name="value">1</property>
    <property name="lower">0.10000000000000001</property>
    <property name="upper">2</property>
    <property name="step_increment">0.050000000000000003</property>
    <property name="page_increment">0.10000000000000001</property>
    <property name="page_size">0.10000000000000001</property>
  </object>
  <object class="GtkAdjustment" id="adj_contrast">
    <property name="value">1</property>
    <property name="lower">0.10000000000000001</property>
    <property name="upper">2</property>
    <property name="step_increment">0.050000000000000003</property>
    <property name="page_increment">0.10000000000000001</property>
    <property name="page_size">0.10000000000000001</property>
  </object>
  <object class="GtkAdjustment" id="adj_gamma">
    <property name="value">1</property>
    <property name="lower">0.10000000000000001</property>
    <property name="upper">3</property>
    <property name="step_increment">0.050000000000000003</property>
    <property name="page_increment">0.10000000000000001</property>
    <property name="page_size">0.10000000000000001</property>
  </object>
  <object class="GtkAdjustment" id="adj_pausebrightness">
    <property name="value">0.65000000000000002</property>
    <property name="upper">1</property>
    <property name="step_increment">0.050000000000000003</property>
    <property name="page_increment">0.050000000000000003</property>
    <property name="page_size">0.10000000000000001</property>
  </object>
  <object class="GtkAdjustment" id="adj_beamwidth">
    <property name="value">1</property>
    <property name="upper">100</property>
    <property name="step_increment">1</property>
    <property name="page_increment">10</property>
    <property name="page_size">10</property>
  </object>
  <object class="GtkListStore" id="model_samplerate">
    <columns>
      <!-- column-name gchararray -->
      <column type="gchararray"/>
    </columns>
    <data>
      <row>
        <col id="0" translatable="yes">48000</col>
      </row>
      <row>
        <col id="0" translatable="yes">44100</col>
      </row>
      <row>
        <col id="0" translatable="yes">22050</col>
      </row>
      <row>
        <col id="0" translatable="yes">11025</col>
      </row>
      <row>
        <col id="0" translatable="yes">0</col>
      </row>
    </data>
  </object>
  <object class="GtkListStore" id="model_videomode">
    <columns>
      <!-- column-name gchararray -->
      <column type="gchararray"/>
    </columns>
    <data>
      <row>
        <col id="0" translatable="yes">Software</col>
      </row>
      <row>
        <col id="0" translatable="yes">OpenGL</col>
      </row>
    </data>
  </object>
  <object class="GtkListStore" id="model_yuvmode">
    <columns>
      <!-- column-name gchararray -->
      <column type="gchararray"/>
    </columns>
    <data>
      <row>
        <col id="0" translatable="yes">none</col>
      </row>
      <row>
        <col id="0" translatable="yes">async</col>
      </row>
      <row>
        <col id="0" translatable="yes">yv12</col>
      </row>
      <row>
        <col id="0" translatable="yes">yuy2</col>
      </row>
      <row>
        <col id="0" translatable="yes">yv12x2</col>
      </row>
      <row>
        <col id="0" translatable="yes">yuy2x2</col>
      </row>
    </data>
  </object>
  <object class="GtkListStore" id="model_glslfiltering">
    <columns>
      <!-- column-name gchararray -->
      <column type="gchararray"/>
    </columns>
    <data>
      <row>
        <col id="0" translatable="yes">No filter</col>
      </row>
      <row>
        <col id="0" translatable="yes">Bilinear</col>
      </row>
      <row>
        <col id="0" translatable="yes">Gaussian blur</col>
      </row>
    </data>
  </object>
  <object class="GtkWindow" id="PerformanceWindow">
    <child>
      <object class="GtkVBox" id="PerformanceVBox">
        <property name="visible">True</property>
        <property name="orientation">vertical</property>
        <property name="spacing">6</property>
        <child>
          <object class="GtkLabel" id="label1">
            <property name="visible">True</property>
            <property name="xalign">0</property>
            <property name="label" translatable="yes">&lt;b&gt;Performance&lt;/b&gt;</property>
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
                  <object class="GtkCheckButton" id="preferences_Performance-autoframeskip">
                    <property name="label" translatable="yes">Automatic frameskip</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="tooltip_text" translatable="yes">Automatically skip frames (-autoframeskip)</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="position">0</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkHBox" id="fskiphbox1">
                    <property name="visible">True</property>
                    <child>
                      <object class="GtkLabel" id="frameskip_label">
                        <property name="visible">True</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Frames to skip:</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">False</property>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkSpinButton" id="preferences_Performance-frameskip">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="adjustment">adj_frameskip</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">False</property>
                        <property name="position">1</property>
                      </packing>
                    </child>
                  </object>
                  <packing>
                    <property name="position">1</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_Performance-throttle">
                    <property name="label" translatable="yes">Throttle</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="tooltip_text" translatable="yes">Throttle game in sync with real time (-throttle)</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">False</property>
                    <property name="position">2</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_Performance-sleep">
                    <property name="label" translatable="yes">Sleep</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="tooltip_text" translatable="yes">Sleeping when idle (-sleep)</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="position">3</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkHBox" id="hbox2">
                    <property name="visible">True</property>
                    <property name="spacing">6</property>
                    <child>
                      <object class="GtkLabel" id="speed_label">
                        <property name="visible">True</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Speed:</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">False</property>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkHScale" id="preferences_Performance-speed">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="tooltip_text" translatable="yes">Control speed of gameplay; lower values are slower (-speed)</property>
                        <property name="adjustment">adj_speed</property>
                        <property name="restrict_to_fill_level">False</property>
                        <property name="fill_level">4</property>
                      </object>
                      <packing>
                        <property name="position">1</property>
                      </packing>
                    </child>
                  </object>
                  <packing>
                    <property name="position">4</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_Performance-refreshspeed">
                    <property name="label" translatable="yes">Refresh speed</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="tooltip_text" translatable="yes">Adjust gameplay speed to keep refresh rate lower than the screen (-refreshspeed)</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">False</property>
                    <property name="position">5</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_Performance-multithreading">
                    <property name="label" translatable="yes">Enable multithreading</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="tooltip_text" translatable="yes">Perform rendering and blitting on a separate thread (-multithreading)</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">False</property>
                    <property name="position">6</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_Performance-sdlvideofps">
                    <property name="label" translatable="yes">Show FPS</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="tooltip_text" translatable="yes">Show the Frames per Second count (-sdlvideofps)</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">False</property>
                    <property name="position">7</property>
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
  </object>
  <object class="GtkWindow" id="SoundWindow">
    <child>
      <object class="GtkVBox" id="SoundVBox">
        <property name="visible">True</property>
        <property name="orientation">vertical</property>
        <property name="spacing">6</property>
        <child>
          <object class="GtkLabel" id="label3">
            <property name="visible">True</property>
            <property name="xalign">0</property>
            <property name="label" translatable="yes">&lt;b&gt;Sound&lt;/b&gt;</property>
            <property name="use_markup">True</property>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="fill">False</property>
            <property name="position">0</property>
          </packing>
        </child>
        <child>
          <object class="GtkAlignment" id="alignment2">
            <property name="visible">True</property>
            <property name="left_padding">12</property>
            <child>
              <object class="GtkVBox" id="vbox3">
                <property name="visible">True</property>
                <property name="orientation">vertical</property>
                <property name="spacing">6</property>
                <child>
                  <object class="GtkCheckButton" id="preferences_Sound-sound">
                    <property name="label" translatable="yes">Enable sound</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="tooltip_text" translatable="yes">Enable sound output (-sound)</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">False</property>
                    <property name="position">0</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkAlignment" id="alignment3">
                    <property name="visible">True</property>
                    <property name="left_padding">12</property>
                    <child>
                      <object class="GtkVBox" id="vbox4">
                        <property name="visible">True</property>
                        <property name="orientation">vertical</property>
                        <property name="spacing">6</property>
                        <child>
                          <object class="GtkCheckButton" id="preferences_Sound-samples">
                            <property name="label" translatable="yes">Enable samples</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">False</property>
                            <property name="tooltip_text" translatable="yes">Enable samples if available (-samples)</property>
                            <property name="draw_indicator">True</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="fill">False</property>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkHBox" id="hbox3">
                            <property name="visible">True</property>
                            <property name="spacing">6</property>
                            <child>
                              <object class="GtkLabel" id="label4">
                                <property name="visible">True</property>
                                <property name="xalign">0</property>
                                <property name="label" translatable="yes">Sample rate</property>
                              </object>
                              <packing>
                                <property name="expand">False</property>
                                <property name="fill">False</property>
                                <property name="position">0</property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkComboBox" id="preferences_Sound-samplerate">
                                <property name="visible">True</property>
                                <property name="tooltip_text" translatable="yes">Sample rate</property>
                                <property name="model">model_samplerate</property>
                                <child>
                                  <object class="GtkCellRendererText" id="renderer1"/>
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
                            <property name="position">1</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkHBox" id="hbox4">
                            <property name="visible">True</property>
                            <property name="spacing">6</property>
                            <child>
                              <object class="GtkLabel" id="label6">
                                <property name="visible">True</property>
                                <property name="xalign">0</property>
                                <property name="label" translatable="yes">Volume</property>
                              </object>
                              <packing>
                                <property name="expand">False</property>
                                <property name="fill">False</property>
                                <property name="position">0</property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkHScale" id="preferences_Sound-volume">
                                <property name="visible">True</property>
                                <property name="can_focus">True</property>
                                <property name="tooltip_text" translatable="yes">Sound volume in decibels (-volume)</property>
                                <property name="adjustment">adj_volume</property>
                                <property name="restrict_to_fill_level">False</property>
                              </object>
                              <packing>
                                <property name="position">1</property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkLabel" id="label15">
                                <property name="visible">True</property>
                                <property name="xalign">0</property>
                                <property name="label" translatable="yes">dB</property>
                              </object>
                              <packing>
                                <property name="position">2</property>
                              </packing>
                            </child>
                          </object>
                          <packing>
                            <property name="position">2</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkHBox" id="hbox7">
                            <property name="visible">True</property>
                            <property name="spacing">6</property>
                            <child>
                              <object class="GtkLabel" id="label32">
                                <property name="visible">True</property>
                                <property name="label" translatable="yes">Audio latency</property>
                              </object>
                              <packing>
                                <property name="expand">False</property>
                                <property name="fill">False</property>
                                <property name="position">0</property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkSpinButton" id="preferences_Sound-audio_latency">
                                <property name="visible">True</property>
                                <property name="can_focus">True</property>
                                <property name="tooltip_text" translatable="yes">Set audio latency (increase to reduce glitches, decrease for responsiveness (-audio_latency)</property>
                                <property name="adjustment">ad_audiolatency</property>
                              </object>
                              <packing>
                                <property name="expand">False</property>
                                <property name="fill">False</property>
                                <property name="position">1</property>
                              </packing>
                            </child>
                          </object>
                          <packing>
                            <property name="position">3</property>
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
                    <property name="fill">False</property>
                    <property name="position">1</property>
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
    </child>
  </object>
  <object class="GtkWindow" id="DebuggingWindow">
    <child>
      <object class="GtkVBox" id="DebuggingVBox">
        <property name="visible">True</property>
        <property name="orientation">vertical</property>
        <property name="spacing">6</property>
        <child>
          <object class="GtkLabel" id="label27">
            <property name="visible">True</property>
            <property name="xalign">0</property>
            <property name="label" translatable="yes">&lt;b&gt;Debugging&lt;/b&gt;</property>
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
              <object class="GtkVBox" id="vbox6">
                <property name="visible">True</property>
                <property name="orientation">vertical</property>
                <property name="spacing">6</property>
                <child>
                  <object class="GtkVBox" id="vbox7">
                    <property name="visible">True</property>
                    <property name="orientation">vertical</property>
                    <property name="spacing">6</property>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Debugging-log">
                        <property name="label" translatable="yes">Enable logging</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="tooltip_text" translatable="yes">Generate an error.log file (-log)</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkAlignment" id="alignment10">
                        <property name="visible">True</property>
                        <property name="left_padding">12</property>
                        <child>
                          <object class="GtkCheckButton" id="preferences_Debugging-verbose">
                            <property name="label" translatable="yes">Verbose logging</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">False</property>
                            <property name="tooltip_text" translatable="yes">Display additional diagnostic information (-verbose)</property>
                            <property name="draw_indicator">True</property>
                          </object>
                        </child>
                      </object>
                      <packing>
                        <property name="position">1</property>
                      </packing>
                    </child>
                  </object>
                  <packing>
                    <property name="position">0</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_Debugging-update_in_pause">
                    <property name="label" translatable="yes">Update in pause</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="tooltip_text" translatable="yes">Keep calling video updates whilst paused (-update_in_pause)</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">False</property>
                    <property name="position">1</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkVBox" id="vbox8">
                    <property name="visible">True</property>
                    <property name="orientation">vertical</property>
                    <property name="spacing">6</property>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Debugging-debug">
                        <property name="label" translatable="yes">Enable debugger</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="tooltip_text" translatable="yes">Enable/disable debugger (-debug)</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkAlignment" id="alignment11">
                        <property name="visible">True</property>
                        <property name="left_padding">12</property>
                        <child>
                          <object class="GtkVBox" id="vbox19">
                            <property name="visible">True</property>
                            <property name="orientation">vertical</property>
                            <property name="spacing">6</property>
                            <child>
                              <object class="GtkHBox" id="hbox5">
                                <property name="visible">True</property>
                                <property name="spacing">6</property>
                                <child>
                                  <object class="GtkLabel" id="label28">
                                    <property name="visible">True</property>
                                    <property name="xalign">0</property>
                                    <property name="label" translatable="yes">Debug script:</property>
                                  </object>
                                  <packing>
                                    <property name="expand">False</property>
                                    <property name="fill">False</property>
                                    <property name="position">0</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkEntry" id="preferences_Debugging-debugscript">
                                    <property name="visible">True</property>
                                    <property name="can_focus">True</property>
                                    <property name="tooltip_text" translatable="yes">Script for debugger (-debugscript)</property>
                                  </object>
                                  <packing>
                                    <property name="expand">False</property>
                                    <property name="fill">False</property>
                                    <property name="position">1</property>
                                  </packing>
                                </child>
                              </object>
                              <packing>
                                <property name="position">0</property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkCheckButton" id="preferences_Debugging-oslog">
                                <property name="label" translatable="yes">Output error.log data to debugger</property>
                                <property name="visible">True</property>
                                <property name="can_focus">True</property>
                                <property name="receives_default">False</property>
                                <property name="tooltip_text" translatable="yes">Output error.log data to the system debugger (-oslog)</property>
                                <property name="draw_indicator">True</property>
                              </object>
                              <packing>
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
  </object>
  <object class="GtkWindow" id="MiscWindow">
    <child>
      <object class="GtkVBox" id="MiscVBox">
        <property name="visible">True</property>
        <property name="orientation">vertical</property>
        <property name="spacing">6</property>
        <child>
          <object class="GtkLabel" id="label38">
            <property name="visible">True</property>
            <property name="xalign">0</property>
            <property name="label" translatable="yes">&lt;b&gt;Miscellaneous Options&lt;/b&gt;</property>
            <property name="use_markup">True</property>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="fill">False</property>
            <property name="position">0</property>
          </packing>
        </child>
        <child>
          <object class="GtkAlignment" id="alignment17">
            <property name="visible">True</property>
            <property name="left_padding">12</property>
            <child>
              <object class="GtkVBox" id="vbox13">
                <property name="visible">True</property>
                <property name="orientation">vertical</property>
                <property name="spacing">6</property>
                <child>
                  <object class="GtkHBox" id="hbox8">
                    <property name="visible">True</property>
                    <property name="spacing">6</property>
                    <child>
                      <object class="GtkLabel" id="label39">
                        <property name="visible">True</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">BIOS name</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">False</property>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkEntry" id="preferences_Misc-bios">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="position">1</property>
                      </packing>
                    </child>
                  </object>
                  <packing>
                    <property name="position">0</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_Misc-cheat">
                    <property name="label" translatable="yes">Enable cheat mode</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="tooltip_text" translatable="yes">Enable cheats in games (-cheat)</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">False</property>
                    <property name="position">1</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_Misc-skip_gameinfo">
                    <property name="label" translatable="yes">Skip gameinfo</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="tooltip_text" translatable="yes">Skip the ROM information screen on launch (-skip_gameinfo)</property>
                    <property name="draw_indicator">True</property>
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
        <child>
          <object class="GtkLabel" id="label37">
            <property name="visible">True</property>
            <property name="xalign">0</property>
            <property name="label" translatable="yes">&lt;b&gt;State and Playback Options&lt;/b&gt;</property>
            <property name="use_markup">True</property>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="fill">False</property>
            <property name="position">2</property>
          </packing>
        </child>
        <child>
          <object class="GtkAlignment" id="alignment20">
            <property name="visible">True</property>
            <property name="left_padding">12</property>
            <child>
              <object class="GtkVBox" id="vbox18">
                <property name="visible">True</property>
                <property name="orientation">vertical</property>
                <property name="spacing">6</property>
                <child>
                  <object class="GtkCheckButton" id="preferences_Playback-autosave">
                    <property name="label" translatable="yes">Automatically save and load state</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="tooltip_text" translatable="yes">Automatically save and load the current state (-autosave)</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">False</property>
                    <property name="position">0</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkTable" id="table2">
                    <property name="visible">True</property>
                    <property name="n_rows">3</property>
                    <property name="n_columns">2</property>
                    <property name="column_spacing">6</property>
                    <property name="row_spacing">6</property>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Playback-mngwrite-toggle">
                        <property name="label" translatable="yes">Record MNG</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="tooltip_text" translatable="yes">Records a .mng movie to the Snapshot directory (-recordmng)</property>
                        <property name="xalign">0</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="x_options"></property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Playback-aviwrite-toggle">
                        <property name="label" translatable="yes">Record AVI</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="tooltip_text" translatable="yes">Records a .avi movie to the Snapshot directory (-recordavi)</property>
                        <property name="xalign">0</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="top_attach">1</property>
                        <property name="bottom_attach">2</property>
                        <property name="x_options"></property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Playback-wavwrite-toggle">
                        <property name="label" translatable="yes">Record WAV</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="tooltip_text" translatable="yes">Records a .wav sound file to the Snapshot directory (-recordwav)</property>
                        <property name="xalign">0</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="top_attach">2</property>
                        <property name="bottom_attach">3</property>
                        <property name="x_options"></property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkEntry" id="preferences_Playback-mngwrite">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                      </object>
                      <packing>
                        <property name="left_attach">1</property>
                        <property name="right_attach">2</property>
                        <property name="x_options"></property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkEntry" id="preferences_Playback-aviwrite">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                      </object>
                      <packing>
                        <property name="left_attach">1</property>
                        <property name="right_attach">2</property>
                        <property name="top_attach">1</property>
                        <property name="bottom_attach">2</property>
                        <property name="x_options"></property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkEntry" id="preferences_Playback-wavwrite">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                      </object>
                      <packing>
                        <property name="left_attach">1</property>
                        <property name="right_attach">2</property>
                        <property name="top_attach">2</property>
                        <property name="bottom_attach">3</property>
                        <property name="x_options"></property>
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
            <property name="position">3</property>
          </packing>
        </child>
      </object>
    </child>
  </object>
  <object class="GtkWindow" id="DisplayWindow">
    <child>
      <object class="GtkNotebook" id="DisplayVBox">
        <property name="visible">True</property>
        <property name="can_focus">True</property>
        <child>
          <object class="GtkVBox" id="video_vbox">
            <property name="visible">True</property>
            <property name="orientation">vertical</property>
            <property name="spacing">6</property>
            <child>
              <object class="GtkLabel" id="label19">
                <property name="visible">True</property>
                <property name="xalign">0</property>
                <property name="label" translatable="yes">&lt;b&gt;Video&lt;/b&gt;</property>
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
                  <object class="GtkVBox" id="vbox5">
                    <property name="visible">True</property>
                    <property name="orientation">vertical</property>
                    <property name="spacing">6</property>
                    <child>
                      <object class="GtkHBox" id="hbox6">
                        <property name="visible">True</property>
                        <property name="spacing">6</property>
                        <child>
                          <object class="GtkLabel" id="label14">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Video mode:</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkComboBox" id="preferences_Video-video">
                            <property name="visible">True</property>
                            <property name="tooltip_text" translatable="yes">Whether software (soft) or hardware (OpenGL) acceleration is used (-video)</property>
                            <property name="model">model_videomode</property>
                            <child>
                              <object class="GtkCellRendererText" id="renderer2"/>
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
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkHBox" id="hbox11">
                        <property name="visible">True</property>
                        <child>
                          <object class="GtkLabel" id="label22">
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Number of screens:</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkEntry" id="entry1">
                            <property name="can_focus">True</property>
                            <property name="tooltip_text" translatable="yes">Number of screens to create - MAME only supports 1 (-numscreens)</property>
                            <property name="editable">False</property>
                            <property name="text" translatable="yes">1</property>
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
                      <object class="GtkCheckButton" id="preferences_Video-window">
                        <property name="label" translatable="yes">Run in window</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="tooltip_text" translatable="yes">If this is selected, MAME will run in a window (-window)</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="position">2</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Video-maximize">
                        <property name="label" translatable="yes">Maximize</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="tooltip_text" translatable="yes">Start in maximized window (-maximize)</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="position">3</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Video-keepaspect">
                        <property name="label" translatable="yes">Keep aspect ratio</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="tooltip_text" translatable="yes">Constrain to the proper aspect ratio (-keepaspect)</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="position">4</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Video-unevenstretch">
                        <property name="label" translatable="yes">Allow uneven stretching</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="tooltip_text" translatable="yes">Allow non-integer stretch factors (-unevenstretch)</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="position">5</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkHBox" id="hbox9">
                        <property name="visible">True</property>
                        <property name="spacing">6</property>
                        <child>
                          <object class="GtkLabel" id="label17">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Effect name:</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="fill">False</property>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkComboBox" id="preferences_Video-effect">
                            <property name="visible">True</property>
                            <property name="tooltip_text" translatable="yes">Name of a .png file in the Artwork directory to use for scanlines effect (-effect)</property>
                            <property name="model">model_effect</property>
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
                        <property name="position">6</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkVBox" id="vbox9">
                        <property name="visible">True</property>
                        <child>
                          <object class="GtkLabel" id="label18">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Centre:</property>
                          </object>
                          <packing>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkAlignment" id="alignment5">
                            <property name="visible">True</property>
                            <property name="left_padding">12</property>
                            <child>
                              <object class="GtkHBox" id="hbox10">
                                <property name="visible">True</property>
                                <property name="spacing">6</property>
                                <child>
                                  <object class="GtkCheckButton" id="preferences_Video-centerh">
                                    <property name="label" translatable="yes">Horizontal</property>
                                    <property name="visible">True</property>
                                    <property name="can_focus">True</property>
                                    <property name="receives_default">False</property>
                                    <property name="tooltip_text" translatable="yes">Centre horizontally (-centerh)</property>
                                    <property name="draw_indicator">True</property>
                                  </object>
                                  <packing>
                                    <property name="expand">False</property>
                                    <property name="position">0</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkCheckButton" id="preferences_Video-centerv">
                                    <property name="label" translatable="yes">Vertical</property>
                                    <property name="visible">True</property>
                                    <property name="can_focus">True</property>
                                    <property name="receives_default">False</property>
                                    <property name="tooltip_text" translatable="yes">Centre vertically (-centerv)</property>
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
                            <property name="position">1</property>
                          </packing>
                        </child>
                      </object>
                      <packing>
                        <property name="position">7</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Video-waitvsync">
                        <property name="label" translatable="yes">Reduce tearing effects</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="has_tooltip">True</property>
                        <property name="tooltip_text" translatable="yes">Wait for VBLANK before flipping screens factors (-waitvsync)</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="position">8</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkHBox" id="hbox15">
                        <property name="visible">True</property>
                        <property name="spacing">6</property>
                        <child>
                          <object class="GtkLabel" id="label29">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">YUV mode:</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkComboBox" id="preferences_Video-scalemode">
                            <property name="visible">True</property>
                            <property name="has_tooltip">True</property>
                            <property name="tooltip_text" translatable="yes">Scale mode (-scalemode)</property>
                            <property name="model">model_yuvmode</property>
                            <child>
                              <object class="GtkCellRendererText" id="renderer3"/>
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
                        <property name="position">9</property>
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
          <object class="GtkLabel" id="label2">
            <property name="visible">True</property>
            <property name="label" translatable="yes">Video</property>
          </object>
          <packing>
            <property name="tab_fill">False</property>
          </packing>
        </child>
        <child>
          <object class="GtkVBox" id="artwork_vbox">
            <property name="visible">True</property>
            <property name="spacing">6</property>
            <child>
              <object class="GtkLabel" id="label31">
                <property name="visible">True</property>
                <property name="xalign">0</property>
                <property name="label" translatable="yes">&lt;b&gt;Artwork&lt;/b&gt;</property>
                <property name="use_markup">True</property>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="fill">False</property>
                <property name="position">0</property>
              </packing>
            </child>
            <child>
              <object class="GtkAlignment" id="alignment15">
                <property name="visible">True</property>
                <property name="left_padding">12</property>
                <child>
                  <object class="GtkVBox" id="vbox12">
                    <property name="visible">True</property>
                    <property name="spacing">6</property>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Artwork-artwork_crop">
                        <property name="label" translatable="yes">Crop artwork</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="tooltip_text" translatable="yes">Crop artwork to game screen size (-artwork_crop)</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">False</property>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Artwork-use_backdrops">
                        <property name="label" translatable="yes">Use backdrops</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="tooltip_text" translatable="yes">Use backdrops if artwork is enabled and available (-use_backdrops)</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">False</property>
                        <property name="position">1</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Artwork-use_bezels">
                        <property name="label" translatable="yes">Use bezels</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="tooltip_text" translatable="yes">Use bezels if artwork is enabled and available (-use_bezels)</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">False</property>
                        <property name="position">2</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Artwork-use_overlays">
                        <property name="label" translatable="yes">Use overlays</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="tooltip_text" translatable="yes">Use overlays if artwork is enabled and available (-use_overlays)</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">False</property>
                        <property name="position">3</property>
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
            <property name="position">1</property>
          </packing>
        </child>
        <child type="tab">
          <object class="GtkLabel" id="label5">
            <property name="visible">True</property>
            <property name="label" translatable="yes">Artwork</property>
          </object>
          <packing>
            <property name="position">1</property>
            <property name="tab_fill">False</property>
          </packing>
        </child>
        <child>
          <object class="GtkVBox" id="screen_vbox">
            <property name="visible">True</property>
            <property name="spacing">6</property>
            <child>
              <object class="GtkLabel" id="label8">
                <property name="visible">True</property>
                <property name="xalign">0</property>
                <property name="label" translatable="yes">&lt;b&gt;Screen&lt;/b&gt;</property>
                <property name="use_markup">True</property>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="fill">False</property>
                <property name="position">0</property>
              </packing>
            </child>
            <child>
              <object class="GtkAlignment" id="alignment4">
                <property name="visible">True</property>
                <property name="left_padding">12</property>
                <child>
                  <object class="GtkTable" id="table1">
                    <property name="visible">True</property>
                    <property name="n_rows">4</property>
                    <property name="n_columns">2</property>
                    <property name="column_spacing">6</property>
                    <property name="row_spacing">6</property>
                    <child>
                      <object class="GtkLabel" id="label9">
                        <property name="visible">True</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Brightness</property>
                      </object>
                      <packing>
                        <property name="x_options">GTK_FILL</property>
                        <property name="y_options"></property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkLabel" id="label11">
                        <property name="visible">True</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Contrast</property>
                      </object>
                      <packing>
                        <property name="top_attach">1</property>
                        <property name="bottom_attach">2</property>
                        <property name="x_options">GTK_FILL</property>
                        <property name="y_options"></property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkLabel" id="label12">
                        <property name="visible">True</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Gamma</property>
                      </object>
                      <packing>
                        <property name="top_attach">2</property>
                        <property name="bottom_attach">3</property>
                        <property name="x_options">GTK_FILL</property>
                        <property name="y_options"></property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkLabel" id="label13">
                        <property name="visible">True</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Pause Brightness</property>
                        <property name="wrap">True</property>
                      </object>
                      <packing>
                        <property name="top_attach">3</property>
                        <property name="bottom_attach">4</property>
                        <property name="x_options">GTK_FILL</property>
                        <property name="y_options"></property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkHScale" id="preferences_Screen-brightness">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="tooltip_text" translatable="yes">Brightness correction (-brightness)</property>
                        <property name="adjustment">adj_brightness</property>
                      </object>
                      <packing>
                        <property name="left_attach">1</property>
                        <property name="right_attach">2</property>
                        <property name="x_options">GTK_FILL</property>
                        <property name="y_options"></property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkHScale" id="preferences_Screen-contrast">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="tooltip_text" translatable="yes">Contrast correction (-contrast)</property>
                        <property name="adjustment">adj_contrast</property>
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
                      <object class="GtkHScale" id="preferences_Screen-gamma">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="tooltip_text" translatable="yes">Gamma correction (-gamma)</property>
                        <property name="adjustment">adj_gamma</property>
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
                      <object class="GtkHScale" id="preferences_Screen-pause_brightness">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="tooltip_text" translatable="yes">Brightness when paused (-pause_brightness)</property>
                        <property name="adjustment">adj_pausebrightness</property>
                      </object>
                      <packing>
                        <property name="left_attach">1</property>
                        <property name="right_attach">2</property>
                        <property name="top_attach">3</property>
                        <property name="bottom_attach">4</property>
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
          <packing>
            <property name="position">2</property>
          </packing>
        </child>
        <child type="tab">
          <object class="GtkLabel" id="label7">
            <property name="visible">True</property>
            <property name="label" translatable="yes">Screen</property>
          </object>
          <packing>
            <property name="position">2</property>
            <property name="tab_fill">False</property>
          </packing>
        </child>
        <child>
          <object class="GtkVBox" id="rotation_vbox">
            <property name="visible">True</property>
            <property name="spacing">6</property>
            <child>
              <object class="GtkLabel" id="label24">
                <property name="visible">True</property>
                <property name="xalign">0</property>
                <property name="label" translatable="yes">&lt;b&gt;Rotation&lt;/b&gt;</property>
                <property name="use_markup">True</property>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="fill">False</property>
                <property name="position">0</property>
              </packing>
            </child>
            <child>
              <object class="GtkAlignment" id="alignment8">
                <property name="visible">True</property>
                <property name="left_padding">12</property>
                <child>
                  <object class="GtkVBox" id="vbox10">
                    <property name="visible">True</property>
                    <property name="spacing">6</property>
                    <child>
                      <object class="GtkHBox" id="hbox14">
                        <property name="visible">True</property>
                        <property name="spacing">6</property>
                        <child>
                          <object class="GtkCheckButton" id="preferences_Rotation-rol">
                            <property name="label" translatable="yes">Rotate left</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">False</property>
                            <property name="tooltip_text" translatable="yes">Rotate screen anticlockwise 90 degrees (-rol)</property>
                            <property name="draw_indicator">True</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkCheckButton" id="preferences_Rotation-ror">
                            <property name="label" translatable="yes">Rotate right</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">False</property>
                            <property name="tooltip_text" translatable="yes">Rotate screen clockwise 90 degrees (-ror)</property>
                            <property name="draw_indicator">True</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
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
                      <object class="GtkVBox" id="vbox11">
                        <property name="visible">True</property>
                        <property name="spacing">6</property>
                        <child>
                          <object class="GtkLabel" id="label25">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Automatic Rotation</property>
                          </object>
                          <packing>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkAlignment" id="alignment12">
                            <property name="visible">True</property>
                            <property name="left_padding">12</property>
                            <child>
                              <object class="GtkHBox" id="hbox12">
                                <property name="visible">True</property>
                                <property name="spacing">6</property>
                                <child>
                                  <object class="GtkCheckButton" id="preferences_Rotation-autorol">
                                    <property name="label" translatable="yes">Rotate left</property>
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
                                  <object class="GtkCheckButton" id="preferences_Rotation-autoror">
                                    <property name="label" translatable="yes">Rotate right</property>
                                    <property name="visible">True</property>
                                    <property name="can_focus">True</property>
                                    <property name="receives_default">False</property>
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
                            <property name="position">1</property>
                          </packing>
                        </child>
                      </object>
                      <packing>
                        <property name="position">1</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkVBox" id="vbox14">
                        <property name="visible">True</property>
                        <property name="spacing">6</property>
                        <child>
                          <object class="GtkLabel" id="label26">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Flip</property>
                          </object>
                          <packing>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkAlignment" id="alignment13">
                            <property name="visible">True</property>
                            <property name="left_padding">12</property>
                            <child>
                              <object class="GtkHBox" id="hbox13">
                                <property name="visible">True</property>
                                <property name="spacing">6</property>
                                <child>
                                  <object class="GtkCheckButton" id="flipx">
                                    <property name="label" translatable="yes">Left-right</property>
                                    <property name="visible">True</property>
                                    <property name="can_focus">True</property>
                                    <property name="receives_default">False</property>
                                    <property name="tooltip_text" translatable="yes">Flip screen left-right (-flipx)</property>
                                    <property name="draw_indicator">True</property>
                                  </object>
                                  <packing>
                                    <property name="expand">False</property>
                                    <property name="position">0</property>
                                  </packing>
                                </child>
                                <child>
                                  <object class="GtkCheckButton" id="flipy">
                                    <property name="label" translatable="yes">Up-down</property>
                                    <property name="visible">True</property>
                                    <property name="can_focus">True</property>
                                    <property name="receives_default">False</property>
                                    <property name="tooltip_text" translatable="yes">Flip screen upside-down (-flipy)</property>
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
                            <property name="position">1</property>
                          </packing>
                        </child>
                      </object>
                      <packing>
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
            <property name="position">3</property>
          </packing>
        </child>
        <child type="tab">
          <object class="GtkLabel" id="label10">
            <property name="visible">True</property>
            <property name="label" translatable="yes">Rotation</property>
          </object>
          <packing>
            <property name="position">3</property>
            <property name="tab_fill">False</property>
          </packing>
        </child>
        <child>
          <object class="GtkVBox" id="vector_vbox">
            <property name="visible">True</property>
            <property name="spacing">6</property>
            <child>
              <object class="GtkLabel" id="label20">
                <property name="visible">True</property>
                <property name="xalign">0</property>
                <property name="label" translatable="yes">&lt;b&gt;Vector&lt;/b&gt;</property>
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
                  <object class="GtkVBox" id="vbox1">
                    <property name="visible">True</property>
                    <property name="spacing">6</property>
                    <child>
                      <object class="GtkCheckButton" id="preferences_Vector-antialias">
                        <property name="label" translatable="yes">Antialias</property>
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="receives_default">False</property>
                        <property name="tooltip_text" translatable="yes">Use antialias when drawing vectors (-antialias)</property>
                        <property name="draw_indicator">True</property>
                      </object>
                      <packing>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkTable" id="table4">
                        <property name="visible">True</property>
                        <property name="n_rows">2</property>
                        <property name="n_columns">2</property>
                        <property name="column_spacing">6</property>
                        <property name="row_spacing">6</property>
                        <child>
                          <object class="GtkLabel" id="label21">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Beam width:</property>
                          </object>
                          <packing>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkLabel" id="label23">
                            <property name="visible">True</property>
                            <property name="xalign">0</property>
                            <property name="label" translatable="yes">Flicker:</property>
                          </object>
                          <packing>
                            <property name="top_attach">1</property>
                            <property name="bottom_attach">2</property>
                            <property name="x_options">GTK_FILL</property>
                            <property name="y_options"></property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkHScale" id="preferences_Vector-beam">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="tooltip_text" translatable="yes">Set vector beam width (-beam)</property>
                            <property name="adjustment">adj_beamwidth</property>
                          </object>
                          <packing>
                            <property name="left_attach">1</property>
                            <property name="right_attach">2</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkHScale" id="preferences_Vector-flicker">
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="tooltip_text" translatable="yes">Set vector flicker effect (-flicker)</property>
                            <property name="adjustment">adj_flicker</property>
                          </object>
                          <packing>
                            <property name="left_attach">1</property>
                            <property name="right_attach">2</property>
                            <property name="top_attach">1</property>
                            <property name="bottom_attach">2</property>
                          </packing>
                        </child>
                      </object>
                      <packing>
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
            <property name="position">4</property>
          </packing>
        </child>
        <child type="tab">
          <object class="GtkLabel" id="label16">
            <property name="visible">True</property>
            <property name="label" translatable="yes">Vector</property>
          </object>
          <packing>
            <property name="position">4</property>
            <property name="tab_fill">False</property>
          </packing>
        </child>
      </object>
    </child>
  </object>
  <object class="GtkWindow" id="InputWindow">
    <child>
      <object class="GtkVBox" id="InputVBox">
        <property name="visible">True</property>
        <property name="orientation">vertical</property>
        <property name="spacing">6</property>
        <child>
          <object class="GtkLabel" id="label30">
            <property name="visible">True</property>
            <property name="xalign">0</property>
            <property name="label" translatable="yes">&lt;b&gt;Input&lt;/b&gt;</property>
            <property name="use_markup">True</property>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="fill">False</property>
            <property name="position">0</property>
          </packing>
        </child>
        <child>
          <object class="GtkAlignment" id="alignment14">
            <property name="visible">True</property>
            <property name="bottom_padding">6</property>
            <property name="left_padding">12</property>
            <property name="right_padding">6</property>
            <child>
              <object class="GtkVBox" id="vbox16">
                <property name="visible">True</property>
                <property name="orientation">vertical</property>
                <property name="spacing">6</property>
                <child>
                  <object class="GtkHBox" id="hbox16">
                    <property name="visible">True</property>
                    <property name="spacing">6</property>
                    <child>
                      <object class="GtkLabel" id="label33">
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Controller directory</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">False</property>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkEntry" id="ctrlr">
                        <property name="can_focus">True</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">False</property>
                        <property name="position">1</property>
                      </packing>
                    </child>
                  </object>
                  <packing>
                    <property name="position">0</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_Input-mouse">
                    <property name="label" translatable="yes">Enable mouse</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">False</property>
                    <property name="position">1</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_Input-joystick">
                    <property name="label" translatable="yes">Enable joystick</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">False</property>
                    <property name="position">2</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_Input-lightgun">
                    <property name="label" translatable="yes">Enable lightgun</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">False</property>
                    <property name="position">3</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkLabel" id="label34">
                    <property name="visible">True</property>
                    <property name="xalign">0</property>
                    <property name="label" translatable="yes">Enable input from multiple connected devices</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">False</property>
                    <property name="position">4</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkAlignment" id="alignment16">
                    <property name="visible">True</property>
                    <property name="left_padding">12</property>
                    <child>
                      <object class="GtkHBox" id="hbox17">
                        <property name="visible">True</property>
                        <property name="spacing">6</property>
                        <child>
                          <object class="GtkCheckButton" id="preferences_Input-multimouse">
                            <property name="label" translatable="yes">Mouse</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">False</property>
                            <property name="tooltip_text" translatable="yes">Enable separate input from each mouse device (-multimouse)</property>
                            <property name="draw_indicator">True</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="fill">False</property>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkCheckButton" id="preferences_Input-multikeyboard">
                            <property name="label" translatable="yes">Keyboard</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">False</property>
                            <property name="tooltip_text" translatable="yes">Enable separate input from each keyboard device (-multikeyboard)</property>
                            <property name="draw_indicator">True</property>
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
                    <property name="position">5</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkLabel" id="label35">
                    <property name="visible">True</property>
                    <property name="xalign">0</property>
                    <property name="label" translatable="yes">Other options</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">False</property>
                    <property name="position">6</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkAlignment" id="alignment18">
                    <property name="visible">True</property>
                    <property name="left_padding">12</property>
                    <child>
                      <object class="GtkVBox" id="vbox17">
                        <property name="visible">True</property>
                        <property name="orientation">vertical</property>
                        <property name="spacing">6</property>
                        <child>
                          <object class="GtkCheckButton" id="preferences_Input-steadykey">
                            <property name="label" translatable="yes">Enable steadykey support</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">False</property>
                            <property name="tooltip_text" translatable="yes">Enable steadykey support (-steadykey)</property>
                            <property name="draw_indicator">True</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="fill">False</property>
                            <property name="position">0</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkCheckButton" id="preferences_Input-offscreen_reload">
                            <property name="label" translatable="yes">Convert lightgun button 2 to offscreen support</property>
                            <property name="visible">True</property>
                            <property name="can_focus">True</property>
                            <property name="receives_default">False</property>
                            <property name="tooltip_text" translatable="yes">Convert lightgun button 2 to offscreen reload (-offscreen_reload)</property>
                            <property name="draw_indicator">True</property>
                          </object>
                          <packing>
                            <property name="expand">False</property>
                            <property name="fill">False</property>
                            <property name="position">1</property>
                          </packing>
                        </child>
                        <child>
                          <object class="GtkHBox" id="hbox1">
                            <property name="visible">True</property>
                            <property name="spacing">6</property>
                            <child>
                              <object class="GtkCheckButton" id="preferences_Input-keymap">
                                <property name="label" translatable="yes">Use keymap file</property>
                                <property name="visible">True</property>
                                <property name="can_focus">True</property>
                                <property name="receives_default">False</property>
                                <property name="tooltip_text" translatable="yes">Enable keymap (-keymap)</property>
                                <property name="draw_indicator">True</property>
                              </object>
                              <packing>
                                <property name="expand">False</property>
                                <property name="fill">False</property>
                                <property name="position">0</property>
                              </packing>
                            </child>
                            <child>
                              <object class="GtkFileChooserButton" id="preferences_Input-keymap_file">
                                <property name="visible">True</property>
                                <property name="tooltip_text" translatable="yes">Keymap file to load (-keymap_file)</property>
                              </object>
                              <packing>
                                <property name="position">1</property>
                              </packing>
                            </child>
                          </object>
                          <packing>
                            <property name="position">2</property>
                          </packing>
                        </child>
                      </object>
                    </child>
                  </object>
                  <packing>
                    <property name="position">7</property>
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
  </object>
  <object class="GtkWindow" id="StatePlaybackWindow">
    <child>
      <object class="GtkVBox" id="StatePlayback">
        <property name="visible">True</property>
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
  <object class="GtkWindow" id="OpenGLWindow">
    <child>
      <object class="GtkVBox" id="OpenGLVBox">
        <property name="visible">True</property>
        <property name="orientation">vertical</property>
        <property name="spacing">6</property>
        <child>
          <object class="GtkLabel" id="label36">
            <property name="visible">True</property>
            <property name="xalign">0</property>
            <property name="label" translatable="yes">&lt;b&gt;OpenGL&lt;/b&gt;</property>
            <property name="use_markup">True</property>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="fill">False</property>
            <property name="position">0</property>
          </packing>
        </child>
        <child>
          <object class="GtkAlignment" id="alignment19">
            <property name="visible">True</property>
            <property name="left_padding">12</property>
            <child>
              <object class="GtkVBox" id="vbox15">
                <property name="visible">True</property>
                <property name="orientation">vertical</property>
                <property name="spacing">6</property>
                <child>
                  <object class="GtkCheckButton" id="preferences_OpenGL-filter">
                    <property name="label" translatable="yes">Filter</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="tooltip_text" translatable="yes">Enable bilinear filtering (-filter)</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">False</property>
                    <property name="position">0</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkHBox" id="hbox22">
                    <property name="visible">True</property>
                    <property name="spacing">6</property>
                    <child>
                      <object class="GtkLabel" id="label40">
                        <property name="visible">True</property>
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Prescale</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">False</property>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkSpinButton" id="preferences_OpenGL-prescale">
                        <property name="visible">True</property>
                        <property name="can_focus">True</property>
                        <property name="tooltip_text" translatable="yes">Scale game graphics before rendering (-prescale)</property>
                        <property name="adjustment">adj_prescale</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">False</property>
                        <property name="position">1</property>
                      </packing>
                    </child>
                  </object>
                  <packing>
                    <property name="position">1</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_OpenGL-gl_notexturerect">
                    <property name="label" translatable="yes">Disable OpenGL GL_ARB_texture_rectangle</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="tooltip_text" translatable="yes">Disable OpenGL GL_ARB_texture_rectangle (-gl_notexturerect)</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">False</property>
                    <property name="position">2</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_OpenGL-gl_forcepow2texture">
                    <property name="label" translatable="yes">Force power of two textures</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="tooltip_text" translatable="yes">Force power of two textures (gl_forcepow2texture)</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">False</property>
                    <property name="position">3</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_OpenGL-gl_vbo">
                    <property name="label" translatable="yes">Enable OpenGL VBO</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="tooltip_text" translatable="yes">Enable OpenGL VBO if available (-gl_vbo)</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">False</property>
                    <property name="position">4</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_OpenGL-gl_pbo">
                    <property name="label" translatable="yes">Enable OpenGL VBO</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="tooltip_text" translatable="yes">Enable OpenGL PBO if available (-gl_pbo)</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">False</property>
                    <property name="position">5</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_OpenGL-gl_glsl">
                    <property name="label" translatable="yes">Enable OpenGL GLSL</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="tooltip_text" translatable="yes">Enable OpenGL GLSL filtering if available (-gl_glsl)</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">False</property>
                    <property name="position">6</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkHBox" id="hbox21">
                    <property name="visible">True</property>
                    <property name="spacing">6</property>
                    <child>
                      <object class="GtkLabel" id="label41">
                        <property name="xalign">0</property>
                        <property name="label" translatable="yes">Use GLSL filtering</property>
                      </object>
                      <packing>
                        <property name="expand">False</property>
                        <property name="fill">False</property>
                        <property name="position">0</property>
                      </packing>
                    </child>
                    <child>
                      <object class="GtkComboBox" id="preferences_OpenGL-gl_glsl_filter">
                        <property name="visible">True</property>
                        <property name="model">model_glslfiltering</property>
                        <child>
                          <object class="GtkCellRendererText" id="renderer4"/>
                          <attributes>
                            <attribute name="text">0</attribute>
                          </attributes>
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
                    <property name="position">7</property>
                  </packing>
                </child>
                <child>
                  <object class="GtkCheckButton" id="preferences_OpenGL-gl_glsl_vid_attr">
                    <property name="label" translatable="yes">Let OpenGL manage brightness and contrast</property>
                    <property name="visible">True</property>
                    <property name="can_focus">True</property>
                    <property name="receives_default">False</property>
                    <property name="tooltip_text" translatable="yes">Enable OpenGL GLSL handling of brightness and contrast (-gl_glsl_vid_attr)</property>
                    <property name="draw_indicator">True</property>
                  </object>
                  <packing>
                    <property name="expand">False</property>
                    <property name="fill">False</property>
                    <property name="position">8</property>
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
  </object>
  <object class="GtkListStore" id="model_effect">
    <columns>
      <!-- column-name gchararray -->
      <column type="gchararray"/>
    </columns>
  </object>
</interface>
