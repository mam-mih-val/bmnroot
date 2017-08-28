<unidbparser_schema>

<search name="table"/>
<move down="1"/>
<cycle child="tr" table_name="run_" skip="1">
 <element action="skip"/>
 <element action="multi">
  <subelement action="write" column_name="file_path" type="string"/>
  <subelement action="parse" start_index="3" parse_type="int" column_name="run_number" type="int"/> 
 </element>
 <element action="write" column_name="event_count" type="int"/>
 <element action="write" column_name="start_datetime" type="datetime"/>
 <element action="write" column_name="end_datetime" type="datetime"/>
 <element action="skip"/>
 <element action="skip"/>
 <element action="skip"/>
</cycle>

</unidbparser_schema>
