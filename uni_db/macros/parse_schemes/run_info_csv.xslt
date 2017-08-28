<unidbparser_schema>

<cycle table_name="run_" skip="2">
 <element action="update" column_name="run_number" type="int"/>
 <element action="skip"/>
 <element action="skip"/>
 <element action="write" column_name="field_current" type="int"/>
 <element action="skip"/>
 <element action="parse" start_index="0" parse_type="double" column_name="file_size" type="double"/>
</cycle>

</unidbparser_schema>
