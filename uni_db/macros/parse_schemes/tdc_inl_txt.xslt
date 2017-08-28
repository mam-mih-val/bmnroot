<unidbparser_schema>

<cycle table_name="detector_parameter" delimiter="\t">
 <element action="multi">
  <subelement action="parse" parse_type="value:TOF1" column_name="detector_name" type="string"/>
  <subelement action="parse" parse_type="value:6" column_name="parameter_id" type="int"/>
  <subelement action="parse" parse_type="value:12" column_name="start_run" type="int"/>
  <subelement action="parse" parse_type="value:688" column_name="end_run" type="int"/>
  <subelement action="parse" parse_type="value:0168fdca" column_name="dc_serial" type="hex"/>
  <subelement action="parse" parse_type="counter" column_name="channel" type="int"/>
  <subelement action="parse" parse_type="double_array" delimiter=" " column_name="parameter_value" type="binary"/>
 </element>
</cycle>

</unidbparser_schema>
