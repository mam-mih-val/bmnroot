-- Unified Database
-- createdb bmn_db;
-- SET bytea_output = 'escape';

-- ALTER TABLE public.detector_parameter DISABLE TRIGGER USER
-- ALTER TABLE public.run_geometry DISABLE TRIGGER USER
-- drop schema public cascade; create schema public;
-- SHOW search_path;
-- SET search_path TO "$user",new_schema;
-- SET search_path TO "$user",public;

CREATE DOMAIN uint AS bigint;

-- EXPERIMENT RUN PERIODS (SESSIONS)
create table run_period
(
 period_number int primary key,
 start_datetime timestamp not null,
 end_datetime timestamp null
);

-- GEOMETRY PART
-- drop table run_geometry
create table run_geometry
(
 geometry_id serial primary key,
 root_geometry bytea not null
);

-- RUN INFO
create table run_
(
 period_number int references run_period(period_number) on update cascade,
 run_number int,
 file_path varchar(200) not null unique,
 beam_particle varchar(10) not null default ('d'),
 target_particle varchar(10) null,
 energy float null check (energy > 0),
 start_datetime timestamp not null,
 end_datetime timestamp null,
 event_count int null check (event_count >= 0),
-- field_current int null check (field_current >= 0),
 field_voltage float null,
 file_size float null check (file_size > 0),
 file_md5 char(32) null,
 geometry_id int null references run_geometry(geometry_id) on update cascade,
 primary key (period_number, run_number)
);

-- FILES WiTH GENERATOR DATA
create table simulation_file
(
 file_id serial primary key,
 file_path varchar(200) not null unique,
 generator_name varchar(20) not null,
 beam_particle varchar(10) not null,
 target_particle varchar(10) null,
 energy float null check (energy > 0),
 centrality varchar(10) not null,
 event_count int null check (event_count >= 0),
 file_desc varchar(30) null,
 file_size float null check (file_size > 0),
 file_md5 char(32) null
);

-- DETECTORS AND OTHER COMPONENTS
create table detector_
(
 detector_name varchar(10) primary key,
 description varchar(30) null
);

-- COMPONENT PARAMETERS
-- parameter_type: 0 - boolean, 1 - integer, 2 - unsigned integer, 3 - double, 4 - string, 5 - binary, 6 - int+int,
--                 7 - DCH mapping, 8 - GEM mapping, 9 - GEM pedestal mapping, 10 - trigger mapping, 11 - Lorentz shift,
--                 12 - mapping with bool value (serial+channel+bool), 13 - mapping with int, 14 - mapping with double vector
-- drop table parameter_
create table parameter_
(
 parameter_id serial primary key,
 parameter_name varchar(20) not null unique,
 parameter_type int not null,
 is_array boolean not null default (false)
);

-- PARAMETERS' VALUES
-- drop table detector_parameter
create table detector_parameter
(
 value_id serial primary key,
 detector_name varchar(10) not null references detector_(detector_name),
 parameter_id int not null references parameter_(parameter_id),
 start_period int not null,
 start_run int not null,
 end_period int not null,
 end_run int not null,
 value_key int not null,
 parameter_value bytea not null,
 expiry_date timestamp null,
 foreign key (start_period, start_run) references run_(period_number, run_number),
 foreign key (end_period, end_run) references run_(period_number, run_number)
);


-- !!! GET INFORMATION ABOUT TABLES ---
-- SELECT
--    a.attnum as ordinal_position,
--    a.attname as column_name,
--    format_type(a.atttypid, a.atttypmod) as data_type,
--    a.attnotnull as is_nullable,
--    f.adsrc as is_default,
--    coalesce(p.indisprimary, false) as is_primary,
--    coalesce(p.indisunique, false) as is_unique
-- FROM pg_attribute a 
-- LEFT JOIN pg_index p ON p.indrelid = a.attrelid AND a.attnum = ANY(p.indkey)
-- LEFT JOIN pg_description d ON d.objoid  = a.attrelid AND d.objsubid = a.attnum
-- LEFT JOIN pg_attrdef f ON f.adrelid = a.attrelid  AND f.adnum = a.attnum
-- WHERE a.attnum > 0
-- AND NOT a.attisdropped
-- AND a.attrelid = 'detector_parameter'::regclass  -- Your table name here
-- ORDER BY a.attnum;

create index det_name_lower_idx on detector_((lower(detector_name)));
create index par_name_lower_idx on parameter_((lower(parameter_name)));
create index det_name_par_lower_idx on detector_parameter((lower(detector_name)));


-- function to set link to the existed geometry with a given id
CREATE OR REPLACE FUNCTION set_geometry_id() RETURNS TRIGGER AS $$
BEGIN
  IF NEW.geometry_id is NULL then
    --SELECT last_value INTO NEW.geometry_id FROM run_geometry_new_geometry_id_seq;
    NEW.geometry_id = 57;
  END IF;

  RETURN NEW;
END;
$$ LANGUAGE plpgsql;
DROP TRIGGER IF EXISTS fixed_geometry_id ON run_;
CREATE TRIGGER fixed_geometry_id
BEFORE INSERT ON run_ FOR EACH ROW EXECUTE PROCEDURE set_geometry_id();


-- trigger to check correctness of the valid period for a new detector parameter and set row as expired if exist
CREATE OR REPLACE FUNCTION check_valid_period() RETURNS TRIGGER AS $$
DECLARE
  objID integer; valueID integer;
BEGIN
    IF EXISTS(SELECT 1 FROM detector_parameter dp WHERE NEW.detector_name = dp.detector_name and NEW.parameter_id = dp.parameter_id and 
      NEW.value_key = dp.value_key and dp.expiry_date is null and (not (
    ((NEW.end_period < dp.start_period) or ((NEW.end_period = dp.start_period) and (NEW.end_run < dp.start_run))) or
    ((NEW.start_period > dp.end_period) or ((NEW.start_period = dp.end_period) and (NEW.start_run > dp.end_run))))))
    THEN
      SELECT value_id INTO valueID
      FROM detector_parameter dp
      WHERE NEW.detector_name = dp.detector_name and NEW.parameter_id = dp.parameter_id and dp.expiry_date is NULL and
            NEW.start_period = dp.start_period and NEW.end_period = dp.end_period and NEW.start_run = dp.start_run and NEW.end_run = dp.end_run and
            NEW.value_key = dp.value_key;

      IF NOT FOUND THEN
        RAISE EXCEPTION 'The period of new detector parameter overlaps with existing value (id: %)', valueID;
      ELSE
        EXECUTE 'UPDATE detector_parameter SET expiry_date = now()::timestamp WHERE value_id = $1' USING valueID;
      END IF;
    END IF;

  RETURN NEW;
END;
$$ LANGUAGE plpgsql;
DROP TRIGGER IF EXISTS check_par_period ON detector_parameter;
CREATE TRIGGER check_par_period
BEFORE INSERT ON detector_parameter FOR EACH ROW EXECUTE PROCEDURE check_valid_period();


--select "clean_unused_geometry"();
CREATE OR REPLACE FUNCTION clean_unused_geometry() RETURNS integer AS $$
DECLARE
  geometryID integer;
BEGIN  
  FOR geometryID IN SELECT geometry_id FROM run_geometry LOOP
    IF NOT EXISTS (SELECT 1 FROM run_ WHERE geometry_id = geometryID) THEN
      RAISE INFO 'Unused geometry: %', geometryID;
      delete from run_geometry where geometry_id = geometryID;
    END IF;
  END LOOP;

  RETURN 0;
END;
$$ LANGUAGE plpgsql;


-- get double value from 4 or 8 bytes
CREATE OR REPLACE FUNCTION GetDouble4Bytea(d bytea, offset_value int, is_little_endian boolean, isFourBytes boolean) RETURNS text AS $$
DECLARE
  binary_value text; double_value float = 0.0;
  sign text; exponent text; exp int; mantissa text; mantissa_index int = 0;
BEGIN
  -- 4 bytes
  IF isFourBytes THEN
    RAISE INFO 'd: %', d;
    IF is_little_endian THEN
      binary_value = get_byte(d, offset_value+0)::bit(8) || get_byte(d, offset_value+1)::bit(8) || get_byte(d, offset_value+2)::bit(8) || get_byte(d, offset_value+3)::bit(8);
    ELSE
      binary_value = get_byte(d, offset_value+3)::bit(8) || get_byte(d, offset_value+2)::bit(8) || get_byte(d, offset_value+1)::bit(8) || get_byte(d, offset_value+0)::bit(8); 
    END IF;
    RAISE INFO 'binary_value: %', binary_value;

    IF binary_value = '00000000000000000000000000000000' OR binary_value = '10000000000000000000000000000000' THEN -- IEEE754-1985 Zero
      double_value = 0.0;
      RETURN double_value;
    END IF;
    sign = substring(binary_value from 1 for 1);
    exponent = substring(binary_value from 2 for 8);
    mantissa = substring(binary_value from 10 for 23); 

    IF exponent = '11111111' THEN
      IF mantissa = '00000000000000000000000' THEN   -- IEEE754-1985 negative and positive infinity
        IF sign = '1' THEN                    
          RETURN '-Infinity';                    
        ELSE                    
          RETURN 'Infinity';  
        END IF;              
      ELSE
        RETURN 'NaN'; -- IEEE754-1985 Not a number
      END IF; 
    END IF;

    exp = exponent::int;
    IF exp > 126 THEN
      exp = exp - 127;
    ELSE
      exp = -exp;
    END IF;

    WHILE mantissa_index < 24 LOOP
      IF substring(mantissa from mantissa_index for 1) = '1' THEN
        double_value = double_value + power(2, -(mantissa_index));
      END IF;
      mantissa_index = mantissa_index + 1;
    END LOOP;

    RAISE INFO 'dvalue: %, exp: %', double_value, exp;
    double_value = double_value * power(2, exp);
    IF (sign = '1') THEN
      double_value = -double_value;
    END IF;
  -- 8 bytes
  ELSE
    IF is_little_endian THEN
      binary_value = get_byte(d, offset_value+0)::bit(8) || get_byte(d, offset_value+1)::bit(8) || get_byte(d, offset_value+2)::bit(8) || get_byte(d, offset_value+3)::bit(8) 
        || get_byte(d, offset_value+4)::bit(8) || get_byte(d, offset_value+5)::bit(8)  || get_byte(d, offset_value+6)::bit(8)  || get_byte(d, offset_value+7)::bit(8);
    ELSE
      binary_value = get_byte(d, offset_value+7)::bit(8) || get_byte(d, offset_value+6)::bit(8) || get_byte(d, offset_value+5)::bit(8) || get_byte(d, offset_value+4)::bit(8)
        || get_byte(d, offset_value+3)::bit(8) || get_byte(d, offset_value+2)::bit(8) || get_byte(d, offset_value+1)::bit(8) || get_byte(d, offset_value+0)::bit(8); 
    END IF;

    IF binary_value = '0000000000000000000000000000000000000000000000000000000000000000' OR binary_value = '1000000000000000000000000000000000000000000000000000000000000000' THEN -- IEEE754-1985 Zero
      double_value = 0.0;
      RETURN double_value;
    END IF;

    sign = substring(binary_value from 1 for 1);
    exponent = substring(binary_value from 2 for 11);
    mantissa = substring(binary_value from 13 for 52);

    IF exponent = '11111111111' THEN
      IF mantissa = '0000000000000000000000000000000000000000000000000000' THEN   -- IEEE754-1985 negative and positive infinity
        IF sign = '1' THEN
          RETURN '-Infinity';
        ELSE
          RETURN 'Infinity';
        END IF;
      ELSE
        RETURN 'NaN'; -- IEEE754-1985 Not a number
      END IF;
    END IF;

    exp = exponent::bit(11)::int;
    exp = exp - 1023;
    --IF exp > 1022 THEN
    --  exp = exp - 1023;
    --ELSE
    --  exp = -exp;
    --END IF;
    
    WHILE mantissa_index < 53 LOOP
      IF substring(mantissa from mantissa_index for 1) = '1' THEN
        double_value = double_value + power(2, -(mantissa_index));
      END IF;
      mantissa_index = mantissa_index + 1;
    END LOOP;

    double_value = (1+double_value) * power(2, exp);
    IF (sign = '1') THEN
      double_value = -double_value;
    END IF;
  END IF;
  
  RETURN double_value;
END;
$$ LANGUAGE plpgsql;
  
-- output bytea as string
-- parameter_type: 0 - boolean, 1 - integer, 2 - unsigned integer, 3 - double, 4 - string, 5 - binary, 6 - int+int,
--                 7 - DCH mapping, 8 - GEM mapping, 9 - GEM pedestal mapping, 10 - trigger mapping, 11 - Lorentz shift,
--                 12 - mapping with bool value (serial+channel+bool), 13 - mapping with int, 14 - mapping with double vector
CREATE OR REPLACE FUNCTION ConvertBytea2String(par_value bytea, par_type integer, number_limit integer DEFAULT 0, is_little_endian boolean DEFAULT false) RETURNS text AS $$
DECLARE
  bytea_size integer; i integer; j integer;
  r record;
  bool_value boolean; int_value integer; uint_value uint; bigint_value bigint; double_value double precision = 0.0; text_value text = '';
BEGIN
  bytea_size = length(par_value);
  --RAISE INFO 'bytea_size: %', bytea_size;
  i = 0; 
  CASE par_type
  -- convert bytea -> boolean[] -> text
  WHEN 0 THEN
    loop
      IF number_limit <> 0 THEN
        IF i >= number_limit THEN
          text_value = text_value || ' ... (boolean array)';
          EXIT;
        END IF;
      END IF;
      bool_value = get_byte(par_value, i);
      IF bool_value THEN
        text_value = text_value || 'true ';
      ELSE
        text_value = text_value || 'false ';
      END IF;
      bytea_size = bytea_size - 1;
      i = i + 1;
      IF bytea_size < 1 THEN
        EXIT;
      END IF;
    END LOOP;
    RETURN text_value;
  -- convert bytea -> integer[] -> text
  WHEN 1 THEN
    loop
      IF number_limit <> 0 THEN
        IF i >= number_limit*4 THEN
          text_value = text_value || ' ... (integer array)';
          EXIT;
        END IF;
      END IF;
      int_value = (get_byte(par_value, i+3) << 24) + (get_byte(par_value, i+2) << 16) + (get_byte(par_value, i+1) << 8) + get_byte(par_value, i+0);
      text_value = text_value || int_value || ' ';
      bytea_size = bytea_size - 4;
      i = i + 4;
      IF bytea_size < 1 THEN
        EXIT;
      END IF;
    END LOOP;
    RETURN text_value;
  -- convert bytea -> unsigned integer[] -> text
  WHEN 2 THEN
    loop
      IF number_limit <> 0 THEN
        IF i >= number_limit*4 THEN
          text_value = text_value || ' ... (unsigned integer array)';
          EXIT;
        END IF;
      END IF;
      uint_value = (get_byte(par_value, i+3) << 24) + (get_byte(par_value, i+2) << 16) + (get_byte(par_value, i+1) << 8) + get_byte(par_value, i+0);
      text_value = text_value || uint_value || ' ';
      bytea_size = bytea_size - 4;
      i = i + 4;
      IF bytea_size < 1 THEN
        EXIT;
      END IF;
    END LOOP;
    RETURN text_value;
  -- convert bytea -> double[] -> text
  WHEN 3 THEN
    loop
      IF number_limit <> 0 THEN
        IF i >= number_limit*8 THEN
          text_value = text_value || ' ... (double array)';
          EXIT;
        END IF;
      END IF;
      text_value = text_value || GetDouble4Bytea(par_value, i, is_little_endian, false) || ' ';
      bytea_size = bytea_size - 8;
      i = i + 8;
      IF bytea_size < 1 THEN
        EXIT;
      END IF;
    END LOOP;
    RETURN text_value;
  -- convert bytea -> string -> text
  WHEN 4 then
    IF number_limit > 0 THEN
      IF bytea_size > number_limit THEN
          RETURN encode(substring(par_value, 0, number_limit), 'escape') || ' ... (string)';
        END IF;
    END IF;
    RETURN encode(par_value, 'escape');
  -- convert bytea -> binary -> text
  WHEN 5 THEN
    IF number_limit > 0 THEN
      IF (bytea_size-8) > number_limit*2 THEN
          RETURN encode(substring(par_value, 8, number_limit*2), 'hex') || ' ... (binary)';
        END IF;
    END IF;
    RETURN encode(substring(par_value, 8), 'hex');
  -- convert bytea -> (int+int)[] -> text
  WHEN 6 then
    loop
      IF number_limit <> 0 THEN
        IF i >= number_limit*8 THEN
          text_value = text_value || ' ... (int+int)';
          EXIT;
        END IF;
      END IF;
      IF i > 0 THEN
        text_value = text_value || '; ';
      END IF;
      int_value = (get_byte(par_value, i+3) << 24) + (get_byte(par_value, i+2) << 16) + (get_byte(par_value, i+1) << 8) + get_byte(par_value, i+0);
      text_value = text_value || int_value || ' ';
      int_value = (get_byte(par_value, i+7) << 24) + (get_byte(par_value, i+6) << 16) + (get_byte(par_value, i+5) << 8) + get_byte(par_value, i+4);
      text_value = text_value || int_value;
      bytea_size = bytea_size - 8;
      i = i + 8;
      IF bytea_size < 1 THEN
        EXIT;
      END IF;
    END LOOP;
    RETURN text_value;
  -- convert bytea -> DCH Mapping -> text
  WHEN 7 then
    loop
      IF number_limit <> 0 THEN
        IF i >= number_limit*24 THEN
          text_value = text_value || ' ... (DCH map)';
          EXIT;
        END IF;
      END IF;
      IF i > 0 THEN
        text_value = text_value || '; ';
      END IF;
      FOR j in 1..6
      loop
        IF j = 3 THEN
          uint_value = (get_byte(par_value, i+j*4-1) << 24) + (get_byte(par_value, i+j*4-2) << 16) + (get_byte(par_value, i+j*4-3) << 8) + get_byte(par_value, i+j*4-4);
          text_value = text_value || uint_value;
        else
          int_value = (get_byte(par_value, i+j*4-1) << 24) + (get_byte(par_value, i+j*4-2) << 16) + (get_byte(par_value, i+j*4-3) << 8) + get_byte(par_value, i+j*4-4);
          text_value = text_value || int_value;
        END IF;
        IF j < 6 THEN
          text_value = text_value || ' ';
        END IF;
      end loop;
      bytea_size = bytea_size - 24;
      i = i + 24;
      IF bytea_size < 1 THEN
        EXIT;
      END IF;
    END LOOP;
    RETURN text_value;
 -- convert bytea -> GEM Mapping -> text
  WHEN 8 then
    loop
      IF number_limit <> 0 THEN
        IF i >= number_limit*24 THEN
          text_value = text_value || ' ... (GEM map)';
          EXIT;
        END IF;
      END IF;
      IF i > 0 THEN
        text_value = text_value || '; ';
      END IF;
      FOR j in 1..6
      loop
        IF j = 1 THEN
          uint_value = (get_byte(par_value, i+j*4-1) << 24) + (get_byte(par_value, i+j*4-2) << 16) + (get_byte(par_value, i+j*4-3) << 8) + get_byte(par_value, i+j*4-4);
          text_value = text_value || uint_value;
        else
          int_value = (get_byte(par_value, i+j*4-1) << 24) + (get_byte(par_value, i+j*4-2) << 16) + (get_byte(par_value, i+j*4-3) << 8) + get_byte(par_value, i+j*4-4);
          text_value = text_value || int_value;
        END IF;
        IF j < 6 THEN
          text_value = text_value || ' ';
        END IF;
      end loop;
      bytea_size = bytea_size - 24;
      i = i + 24;
      IF bytea_size < 1 THEN
        EXIT;
      END IF;
    END LOOP;
    RETURN text_value;
  -- convert bytea -> GEM Pedestal Mapping -> text
  WHEN 9 then
    loop
      IF number_limit <> 0 THEN
        IF i >= number_limit*16 THEN
          text_value = text_value || ' ... (GEM pedestals)';
          EXIT;
        END IF;
      END IF;
      IF i > 0 THEN
        text_value = text_value || '; ';
      END IF;
      FOR j in 1..4
      loop
        IF j = 1 THEN
          uint_value = (get_byte(par_value, i+j*4-1) << 24) + (get_byte(par_value, i+j*4-2) << 16) + (get_byte(par_value, i+j*4-3) << 8) + get_byte(par_value, i+j*4-4);
          text_value = text_value || uint_value;
        else
          int_value = (get_byte(par_value, i+j*4-1) << 24) + (get_byte(par_value, i+j*4-2) << 16) + (get_byte(par_value, i+j*4-3) << 8) + get_byte(par_value, i+j*4-4);
          text_value = text_value || int_value;
        END IF;
        IF j < 4 THEN
          text_value = text_value || ' ';
        END IF;
      end loop;
      bytea_size = bytea_size - 16;
      i = i + 16;
      IF bytea_size < 1 THEN
        EXIT;
      END IF;
    END LOOP;
    RETURN text_value;
  -- convert bytea -> Trigger Mapping -> text
  WHEN 10 then
    loop
      IF number_limit <> 0 THEN
        IF i >= number_limit*12 THEN
          text_value = text_value || ' ... (Trigger map)';
          EXIT;
        END IF;
      END IF;
      IF i > 0 THEN
        text_value = text_value || '; ';
      END IF;
      uint_value = (get_byte(par_value, i+3) << 24) + (get_byte(par_value, i+2) << 16) + (get_byte(par_value, i+1) << 8) + get_byte(par_value, i+0);
      text_value = text_value || uint_value || ' ';
      uint_value = (get_byte(par_value, i+7) << 24) + (get_byte(par_value, i+6) << 16) + (get_byte(par_value, i+5) << 8) + get_byte(par_value, i+4);
      text_value = text_value || uint_value || ' ';
      int_value = (get_byte(par_value, i+11) << 24) + (get_byte(par_value, i+10) << 16) + (get_byte(par_value, i+9) << 8) + get_byte(par_value, i+8);
      text_value = text_value || int_value;
      bytea_size = bytea_size - 12;
      i = i + 12;
      IF bytea_size < 1 THEN
        EXIT;
      END IF;
    END LOOP;
    RETURN text_value;
  -- convert bytea -> Lorentz Shift -> text
  WHEN 11 then
    loop
      IF number_limit <> 0 THEN
        IF i >= number_limit*28 THEN
          text_value = text_value || ' ... (Lorentz shift)';
          EXIT;
        END IF;
      END IF;
      IF i > 0 THEN
        text_value = text_value || '; ';
      END IF;
      int_value = (get_byte(par_value, i+3) << 24) + (get_byte(par_value, i+2) << 16) + (get_byte(par_value, i+1) << 8) + get_byte(par_value, i+0);
      text_value = text_value || int_value || ' ';
      text_value = text_value || GetDouble4Bytea(par_value, i+4, is_little_endian, false) || ' ';
      text_value = text_value || GetDouble4Bytea(par_value, i+12, is_little_endian, false) || ' ';
      text_value = text_value || GetDouble4Bytea(par_value, i+20, is_little_endian, false);
      bytea_size = bytea_size - 28;
      i = i + 28;
      IF bytea_size < 1 THEN
        EXIT;
      END IF;
    END LOOP;
    RETURN text_value;
  -- convert bytea -> Map to Bool (serial+channel+bool) -> text
  WHEN 12 then
    loop
      IF number_limit <> 0 THEN
        IF i >= number_limit*9 THEN
          text_value = text_value || ' ... (serial+channel+bool)';
          EXIT;
        END IF;
      END IF;
      IF i > 0 THEN
        text_value = text_value || '; ';
      END IF;
      uint_value = (get_byte(par_value, i+3) << 24) + (get_byte(par_value, i+2) << 16) + (get_byte(par_value, i+1) << 8) + get_byte(par_value, i+0);
      text_value = text_value || uint_value || ' ';
      int_value = (get_byte(par_value, i+7) << 24) + (get_byte(par_value, i+6) << 16) + (get_byte(par_value, i+5) << 8) + get_byte(par_value, i+4);
      text_value = text_value || int_value || ' ';
      bool_value = get_byte(par_value, i+8);
      IF bool_value THEN
        text_value = text_value || 'true';
      ELSE
        text_value = text_value || 'false';
      END IF;
      bytea_size = bytea_size - 9;
      i = i + 9;
      IF bytea_size < 1 THEN
        EXIT;
      END IF;
    END LOOP;
    RETURN text_value;
  -- convert bytea -> Map to Int (serial+channel+int) -> text
  WHEN 13 then
    loop
      IF number_limit <> 0 THEN
        IF i >= number_limit*12 THEN
          text_value = text_value || ' ... (serial+channel+int)';
          EXIT;
        END IF;
      END IF;
      IF i > 0 THEN
        text_value = text_value || '; ';
      END IF;
      uint_value = (get_byte(par_value, i+3) << 24) + (get_byte(par_value, i+2) << 16) + (get_byte(par_value, i+1) << 8) + get_byte(par_value, i+0);
      text_value = text_value || uint_value || ' ';
      int_value = (get_byte(par_value, i+7) << 24) + (get_byte(par_value, i+6) << 16) + (get_byte(par_value, i+5) << 8) + get_byte(par_value, i+4);
      text_value = text_value || int_value || ' ';
      int_value = (get_byte(par_value, i+11) << 24) + (get_byte(par_value, i+10) << 16) + (get_byte(par_value, i+9) << 8) + get_byte(par_value, i+8);
      text_value = text_value || int_value;
      bytea_size = bytea_size - 12;
      i = i + 12;
      IF bytea_size < 1 THEN
        EXIT;
      END IF;
    END LOOP;
    RETURN text_value;
  -- convert bytea -> Map to Double vector (serial+channel+int) -> text
  WHEN 14 then
    j = 0;
    loop
      IF number_limit <> 0 THEN
        IF j >= number_limit THEN
          text_value = text_value || ' ... (serial+channel+dvector)';
          EXIT;
        END IF;
      END IF;
      IF i > 0 THEN
        text_value = text_value || '; ';
      END IF;
      uint_value = (get_byte(par_value, i+3) << 24) + (get_byte(par_value, i+2) << 16) + (get_byte(par_value, i+1) << 8) + get_byte(par_value, i+0);
      text_value = text_value || uint_value || ' ';
      int_value = (get_byte(par_value, i+7) << 24) + (get_byte(par_value, i+6) << 16) + (get_byte(par_value, i+5) << 8) + get_byte(par_value, i+4);
      text_value = text_value || int_value || ' ';
      bigint_value = (get_byte(par_value, i+15) << 56) + (get_byte(par_value, i+14) << 48) + (get_byte(par_value, i+13) << 40) + (get_byte(par_value, i+12) << 32) +
                     (get_byte(par_value, i+11) << 24) + (get_byte(par_value, i+10) << 16) + (get_byte(par_value, i+9) << 8) + get_byte(par_value, i+8);
      text_value = text_value || '{' || bigint_value/8 || ' doubles}';
      --loop
      --  text_value = text_value || GetDouble4Bytea(par_value, i+16, is_little_endian, false) || ' ';
      --  bigint_value = bigint_value - 8;
      --  bytea_size = bytea_size - 8;
      --  i = i + 8;
      --  IF bigint_value < 1 THEN
      --    EXIT;
      --  END IF;
      --END LOOP;
      
      bytea_size = bytea_size - 16 - bigint_value;
      i = i + 16 + bigint_value;
      j = j + 1;
      IF bytea_size < 1 THEN
        EXIT;
      END IF;
    END LOOP;
    RETURN text_value;
  ELSE
    RETURN 'data array';
  END CASE;
END;
$$ LANGUAGE plpgsql;

-- ALTER DATABASE bmn_db
-- SET log_statement = 'mod';
