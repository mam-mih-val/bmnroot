<unidbparser_schema>

<cycle table_name="map_2dim" delimiter="\t" skip="1">
 <element action="multi">
  <subelement action="write" column_name="serial_hex" type="string"/> 
  <subelement action="parse" parse_type="counter" column_name="map_row" type="int"/>
  <subelement action="parse" parse_type="value:2" column_name="map_id" type="int"/>
 </element>
 <element action="write" column_name="channel" type="int"/>
 <element action="write" column_name="f_channel" type="int"/>
 <element action="write" column_name="channel_size" type="int"/>
 <element action="write" column_name="x" type="int"/>
 <element action="write" column_name="y" type="int"/>
 <element action="write" column_name="is_connected" type="int"/>
</cycle>

</unidbparser_schema>
