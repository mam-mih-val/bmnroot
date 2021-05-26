<unidbparser_schema>

<cycle table_name="map_1dim" delimiter="\t" skip="2">
 <element action="multi">
  <subelement action="write" column_name="plane" type="string"/>
  <subelement action="parse" parse_type="counter" column_name="map_row" type="int"/>
  <subelement action="parse" parse_type="value:1" column_name="map_id" type="int"/>
 </element>
 <element action="write" column_name="map_group" type="int"/>
 <element action="write" column_name="serial_hex" type="string"/>
 <element action="write" column_name="slot" type="int"/>
 <element action="write" column_name="channel_low" type="int"/>
 <element action="write" column_name="channel_high" type="int"/>
</cycle>

</unidbparser_schema>
