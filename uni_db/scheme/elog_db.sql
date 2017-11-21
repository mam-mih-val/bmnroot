-- drop database bmn_elog;
create database bmn_elog;

--createlang -d bmn_elog plpgsql;

create table person_
(
 person_id serial primary key,
 person_name varchar(30) unique not null,
 is_shift_leader boolean not null default false
);

create table beam_
(
 beam varchar(10) primary key
);

create table target_
(
 target varchar(10) primary key
);

create table type_
(
 type_id serial primary key,
 type_text varchar(30) unique not null
);

create table trigger_
(
 trigger_id serial primary key,
 trigger_info varchar(60) unique not null
);

create table log_record
(
 record_id serial primary key,
 record_date timestamp not null default now(),
 author int null references person_(person_id),
 record_type int not null references type_(type_id),
 run_number int null,
 shift_leader int null references person_(person_id),
 trigger_config int null references trigger_(trigger_id) on update cascade,
 daq_status varchar(70) null,
 sp_41 int null,
 field_comment varchar(70) null,
 beam varchar(10) null references beam_(beam),
 energy float null check (energy > 0),
 target varchar(10) null references target_(target),
 target_width float null,
 record_comment text,
);

create table attachment_
(
 record_id int not null references log_record(record_id),
 attachment_number int not null,	-- start with 1
 file_name varchar(255) not null,
 file_data bytea not null,
 primary key (record_id, attachment_number)
);

CREATE or REPLACE FUNCTION bytea_import(file_path text, p_result out bytea)
AS $$
declare
  l_oid oid;
  r record;
begin
  p_result := '';
  select lo_import(file_path) into l_oid;
  for r in ( select data 
             from pg_largeobject 
             where loid = l_oid 
             order by pageno ) loop
    p_result = p_result || r.data;
  end loop;
  perform lo_unlink(l_oid);
end;
$$ LANGUAGE plpgsql;

CREATE or REPLACE FUNCTION file_upload(file_path text, rec_id int)
RETURNS void AS $$
declare
  att_existed int;
  f_name text;
begin
  -- calculate count of existed attachments for the ELOG ecord
  select count(*) from attachment_ where attachment_.record_id = rec_id
  into att_existed;
  att_existed := att_existed + 1;

  -- extract file name from path
  f_name := regexp_replace(file_path,'^.+[/\]', '');
  
  -- insert attachment as bytea
  insert into attachment_(record_id, attachment_number, file_name, file_data)
  select rec_id, att_existed, f_name, bytea_import(file_path);
end;
$$ LANGUAGE plpgsql;

-- select file_upload('/opt/img/IMG_2016-12-19_234128.jpg', 213);
