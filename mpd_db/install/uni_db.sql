-- Unified experiment database
-- drop schema public cascade; create schema public;
create database mpd_db;

-- MS SQL Server convertion
-- replace serial : int identity
-- replace references : foreign key references
-- replace timestamp : smalltimesttamp
-- replace bytea : blob

-- MySQL convertion
-- replace timestamp : datetime
-- replace boolean : bit
-- replace true : 1
-- replace bytea : blob

-- EXPERIMENT SESSIONS
create table session_
(
 session_number int primary key,
 start_datetime timestamp not null,
 end_datetime timestamp null
);

-- SHIFTS
-- drop table shift
create table shift_
(
 shift_id serial primary key,
 session_number int not null references session_(session_number) on update cascade,
 FIO varchar(40) not null,
 start_datetime timestamp not null,
 end_datetime timestamp not null,
 responsibility varchar(20) null
);

-- DETECTORS AND OTHER COMPONENTS
create table detector_
(
 detector_name varchar(10) primary key,
 manufacturer_name varchar(30) null,
 responsible_person varchar(40) null,
 description varchar(30) null
);

-- DETECTORS' MAPPING
-- map_type: 1 - map_1dim, 2 - map_2dim
create table mapping_
(
 map_id serial primary key,
 map_type int not null
);

-- drop table map_1dim
create table map_1dim
(
 map_id int not null references mapping_(map_id) on update cascade on delete cascade,
 map_row int not null,
 serial_hex varchar(20) not null,
 plane int not null,
 map_group int not null,
 slot int not null,
 channel_low int not null,
 channel_high int not null,
 primary key (map_id, map_row),
 check (channel_high > channel_low)
);

-- drop table map_2dim
create table map_2dim
(
 map_id int not null references mapping_(map_id) on update cascade on delete cascade,
 map_row int not null,
 serial_hex varchar(20) not null,
 channel int not null,
 f_channel int not null,
 channel_size int not null,
 x int not null,
 y int not null,
 is_connected boolean not null default true,
 primary key (map_id, map_row)
);

create table session_detector
(
 session_number int not null references session_(session_number) on update cascade,
 detector_name varchar(10) not null references detector_(detector_name),
 map_id int null references mapping_(map_id) on update cascade on delete cascade,
 primary key (session_number, detector_name)
);

-- GEOMETRY PART
create table run_geometry
(
 geometry_id serial primary key,
 root_geometry bytea not null
);

-- RUN INFO
create table run_
(
 run_number int primary key,
 session_number int references session_(session_number) on update cascade,
 file_path varchar(200) not null unique,
 beam_particle varchar(10) not null default ('d'),
 target_particle varchar(10) null,
 energy float null check (energy > 0),
 start_datetime timestamp not null,
 end_datetime timestamp null,
 event_count int null check (event_count >= 0),
 field_current int null check (field_current >= 0),
 file_size float null check (file_size > 0),
 geometry_id int null references run_geometry(geometry_id) on update cascade
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
 file_size_kb float null check (file_size_kb > 0)
);

-- COMPONENT PARAMETERS
-- parameter_type: 0 - bool, 1-int, 2 - double, 3 - string, 4 - int+int array
create table parameter_
(
 parameter_id serial primary key,
 parameter_name varchar(20) not null unique,
 parameter_type int not null
);

create table detector_parameter
(
 run_number int not null references run_(run_number),
 detector_name varchar(10) not null references detector_(detector_name),
 parameter_id int not null references parameter_(parameter_id),
 parameter_value bytea not null,
 primary key (run_number, detector_name, parameter_id)
);

/*-- GEOMETRY PART (decomposition)
create table geometry_media
(
 media_name varchar(20) primary key,
 density float null,
 radiation_length float null,
 interaction_length float null,
 sensitivity_flag int null,
 field_flag int null,
 field_max float null,
 ang_deviation_max float null,
 step_max float null,
 energy_loss_max float,
 b_crossing_precision float null,
 step_min float null,
 media_desc varchar(50) null
);

create table media_element
(
 element_id serial primary key,
 media_name varchar(20) references geometry_media(media_name),
 atomic_weight float not null,
 atomic_number float not null,
 relative_weight float null
);

-- dictionary of possible geometric shapes
create table geometry_shape
(
 shape_index int primary key,
 shape_name varchar(20) not null,
 shape_desc varchar(40) null
);

-- filling all kinds of geometric shapes
insert into geometry_shape(shape_index, shape_name, shape_desc)
values (0, "box", "box");
insert into geometry_shape(shape_index, shape_name, shape_desc)
values (1, "pgon", "polygone");
insert into geometry_shape(shape_index, shape_name, shape_desc)
values (2, "pcon", "polycone");
insert into geometry_shape(shape_index, shape_name, shape_desc)
values (3, "tube", "cylindrical tube");
insert into geometry_shape(shape_index, shape_name, shape_desc)
values (4, "tubeseg", "cylindrical tube segment");
insert into geometry_shape(shape_index, shape_name, shape_desc)
values (5, "sphere", "sphere");
insert into geometry_shape(shape_index, shape_name, shape_desc)
values (6, "torus", "torus");
insert into geometry_shape(shape_index, shape_name, shape_desc)
values (7, "cone", "conical tube");
insert into geometry_shape(shape_index, shape_name, shape_desc)
values (8, "coneseg", "conical tube segment");
insert into geometry_shape(shape_index, shape_name, shape_desc)
values (9, "trd1", "TRD1 shape");
insert into geometry_shape(shape_index, shape_name, shape_desc)
values (10, "trd2", "TRD2 shape");
insert into geometry_shape(shape_index, shape_name, shape_desc)
values (11, "trap", "G3 TRAP");
insert into geometry_shape(shape_index, shape_name, shape_desc)
values (12, "comb", "boolean composite shape");
insert into geometry_shape(shape_index, shape_name, shape_desc)
values (13, "para", "parallelipeped");
insert into geometry_shape(shape_index, shape_name, shape_desc)
values (14, "arb8", "arbitrary trapezoid with 8 vertices");
insert into geometry_shape(shape_index, shape_name, shape_desc)
values (15, "eltu", "elliptical tube");
insert into geometry_shape(shape_index, shape_name, shape_desc)
values (16, "xtru", "extruded polygon");

create table geometry_node
(
 node_id serial primary key,
 node_name varchar(20) not null,
 parent_node_id int null references geometry_node(node_id),
 media_name varchar(20) references geometry_media(media_name),
 shape_index int references geometry_shape(shape_index)
);

create table component_geometry
(
 geometry_id serial primary key,
 file_id int references raw_data(file_id),
 component_name varchar(10) references component(component_name),
 node_id int references geometry_volume(volume_id)
);

create table geometry_parameter
(
 parameter_type int not null,
 node_id int not null references geometry_node(node_id),
 parameter_value float not null,
 primary key (parameter_type, node_id)
);
*/

-- !!! GET INFORMATION ABOUT TABLES ---
--
-- MYSQL --
-- SHOW KEYS FROM detector_geometry WHERE Key_name = 'PRIMARY';
-- SHOW INDEX FROM detector_geometry WHERE key_name = 'primary';
-- show columns from detector_geometry where extra like '%auto_increment%';
-- SHOW TABLE STATUS FROM `mpd_db` LIKE 'detector_geometry';
-- SELECT ordinal_position, column_name, data_type, (is_nullable = 'YES') AS is_nullable, (extra = 'auto_increment') AS is_identity,
-- 		  (column_key = 'PRI') AS is_primary, (column_key = 'UNI') AS is_unique
-- FROM INFORMATION_SCHEMA.COLUMNS
-- WHERE table_name = 'run'
-- ORDER BY ordinal_position;
--
-- POSTGRES --
-- old version
-- SELECT DISTINCT
--    a.attnum as ordinal_position,
--    a.attname as column_name,
--    format_type(a.atttypid, a.atttypmod) as data_type,
--    a.attnotnull as is_nullable,
--    def.adsrc as is_default,
--    coalesce(i.indisprimary, false) as is_primary,
--    coalesce(i.indisunique, false) as is_unique
-- FROM pg_attribute a
-- JOIN pg_class pgc ON pgc.oid = a.attrelid
-- LEFT JOIN pg_index i ON (pgc.oid = i.indrelid AND a.attnum = ANY(i.indkey))
-- LEFT JOIN pg_description com ON (pgc.oid = com.objoid AND a.attnum = com.objsubid)
-- LEFT JOIN pg_attrdef def ON (a.attrelid = def.adrelid AND a.attnum = def.adnum)
-- WHERE a.attnum > 0 AND pgc.oid = a.attrelid
-- AND pg_table_is_visible(pgc.oid)
-- AND NOT a.attisdropped
-- AND pgc.relname = 'detector_parameter'  
-- ORDER BY a.attnum;
-- current version
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
create index det_name_par_lower_idx on detector_parameter((lower(detector_name)));
create unique index parameter_name_lower_idx on parameter_((lower(parameter_name)));

-- trigger to remove large_object by OID
CREATE OR REPLACE FUNCTION unlink_lo_parameter() RETURNS TRIGGER AS $$
DECLARE
  objID integer;
BEGIN
  objID = OLD.parameter_value;
  PERFORM lo_unlink(objID);
  RETURN OLD;
END;
$$ LANGUAGE plpgsql;
CREATE TRIGGER unlink_large_object_parameter
AFTER UPDATE OR DELETE ON detector_parameter FOR EACH ROW EXECUTE PROCEDURE unlink_lo_parameter();

-- DROP TRIGGER unlink_large_object_geometry ON run_geometry;
CREATE OR REPLACE FUNCTION unlink_lo_geometry() RETURNS TRIGGER AS $$
DECLARE
  objID integer;
BEGIN
  objID = OLD.root_geometry;
  PERFORM lo_unlink(objID);
  RETURN OLD;
END;
$$ LANGUAGE plpgsql;
CREATE TRIGGER unlink_large_object_geometry
AFTER UPDATE OR DELETE ON run_geometry FOR EACH ROW EXECUTE PROCEDURE unlink_lo_geometry();

-- output bytea as string
CREATE OR REPLACE FUNCTION ConvertBytea2String(par_value bytea, par_type integer, is_little_endian boolean) RETURNS text AS $$
DECLARE
  objID integer; lObjFD integer;
  d bytea;
  bool_value boolean; int_value integer; double_value float = 0.0;
  binary_value text; sign text; exponent text; exp int; mantissa text; mantissa_index int = 0;
BEGIN
  CASE par_type
  -- convert bytea -> boolean -> text
  WHEN 0 THEN
    objID = par_value;
    lObjFD = lo_open(objID, x'40000'::int);
    d = loread(lObjFD, 1);
    bool_value = get_byte(d, 0);
    RETURN bool_value;
  -- convert bytea -> integer -> text
  WHEN 1 THEN
    objID = par_value;
    lObjFD = lo_open(objID, x'40000'::int);
    d = loread(lObjFD, 4);
    int_value = (get_byte(d, 3) << 24) + (get_byte(d, 2) << 16) + (get_byte(a, 1) << 8) + get_byte(a, 0);
    RETURN int_value;
  -- convert bytea -> float -> text
  WHEN 2 THEN
    objID = par_value;
    lObjFD = lo_open(objID, x'40000'::int);
    d = loread(lObjFD, 4);

    IF is_little_endian THEN
      binary_value = get_byte(d, 0)::bit(8) || get_byte(d, 1)::bit(8) || get_byte(d, 2)::bit(8) || get_byte(d, 3)::bit(8);
    ELSE
      binary_value:= get_byte(d, 3)::bit(8) || get_byte(d, 2)::bit(8) || get_byte(d, 1)::bit(8) || get_byte(d, 0)::bit(8); 
    END IF;

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
    double_value = double_value * power(2, exp);
    IF (sign = '1') THEN
        double_value = -double_value;
    END IF;

    RETURN double_value;
  -- convert bytea -> text
  WHEN 3 THEN
    objID = par_value;
    lObjFD = lo_open(objID, x'40000'::int);
    d = loread(lObjFD, 1);
    RETURN encode(d, 'escape');
  ELSE
    RETURN 'data array';
  END CASE;
END;
$$ LANGUAGE plpgsql;

insert into session_(session_number, start_datetime, end_datetime)
values (1, '2015-02-22 15:55:12', '2015-02-25 01:11:57');
insert into session_(session_number, start_datetime, end_datetime)
values (2, '2015-03-05 03:34:22', '2015-03-08 14:27:21');
insert into session_(session_number, start_datetime, end_datetime)
values (3, '2015-03-11 20:59:55', '2015-03-15 08:24:29');

insert into detector_(detector_name)
values ('DCH1');
insert into detector_(detector_name)
values ('DCH2');
insert into detector_(detector_name)
values ('TOF1');
insert into detector_(detector_name)
values ('TOF2');
insert into detector_(detector_name)
values ('ZDC');

-- insert into session_detector(session_number, detector_name)
-- values (1, 'DCH1');
-- insert into session_detector(session_number, detector_name)
-- values (1, 'DCH2');
-- insert into session_detector(session_number, detector_name)
-- values (1, 'TOF1');
-- insert into session_detector(session_number, detector_name)
-- values (1, 'TOF2');
-- insert into session_detector(session_number, detector_name)
-- values (1, 'ZDC');
-- 
-- insert into run_(run_number, session_number, file_path, beam_particle, target_particle, energy, start_datetime, end_datetime, event_count)
-- values (12, 1, 'run012.data', 'd', null, 3.5, '2015-02-22 15:55:12', '2015-02-22 15:55:13', 156);
-- insert into run_(run_number, session_number, file_path, beam_particle, target_particle, energy, start_datetime, end_datetime, event_count)
-- values (13, 1, 'run013.data', 'd', null, 3.5, '2015-02-22 16:01:04', '2015-02-22 16:02:56', 5720);
-- 
insert into parameter_(parameter_name, parameter_type)
values ('on', 0);
-- insert into parameter_(parameter_name, parameter_type)
-- values ('noise', 4);