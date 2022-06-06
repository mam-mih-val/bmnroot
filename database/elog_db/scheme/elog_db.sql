-- createlang -d bmn_elog plpgsql;
-- drop database bmn_elog;
-- create database bmn_elog;

-- alter table person_ add column email varchar(30) null
create table person_
(
 person_id serial primary key,
 person_name varchar(30) unique not null,
 email varchar(30) null,
 is_active boolean not null default true
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

-- record_ contains only base fields (specific columns including magnetic field info are defined via eLog platform)
-- specific 'target_width' field remains to combine target info into one web column
create table record_
(
 record_id serial primary key,
 record_date timestamp(0) not null default now()::timestamp,
 shift_leader_id int null references person_(person_id),
 type_id int not null references type_(type_id),
 period_number int null,
 run_number int null,
 trigger_id int null references trigger_(trigger_id) on update cascade,
 daq_status varchar(70) null,
 beam varchar(10) null references beam_(beam) on update cascade,
 energy float null check (energy > 0),
 target varchar(10) null references target_(target) on update cascade,
 target_width float null,
 record_comment text
);

--drop table attachment_
create table attachment_
(
 record_id int not null references record_(record_id),
 attachment_number int not null,	-- start with 1
 file_path varchar(4096) unique not null,
 --file_data bytea not null,
 primary key (record_id, attachment_number)
);

--drop table _notification_
-- event_id = type_.type_id + 100 (Ref. Book changing)
create table _notification_
(
 event_id int not null,
 person_id int not null references person_(person_id) on delete cascade,
 primary key (event_id, person_id)
);

-- TRIGGERS
-- trigger to remove personal notifications, if a corresponding type of records has been removed 
CREATE OR REPLACE FUNCTION delete_notifications() RETURNS TRIGGER AS $$
DECLARE
  valueID integer;
BEGIN
  valueID = OLD.type_id;
  EXECUTE 'DELETE FROM _notification_ WHERE event_id = $1' USING valueID;
  
  RETURN OLD;
END;
$$ LANGUAGE plpgsql;
CREATE TRIGGER delete_record_type
AFTER DELETE ON type_ FOR EACH ROW EXECUTE PROCEDURE delete_notifications();
--drop trigger delete_record_type on type_;

-- FUNCTIONS
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
