create database event_db;

-- drop table software_
create table software_
(
	software_id smallserial primary key,
	software_version varchar (20) not null unique
);

-- drop table storage_
create table storage_
(
	storage_id smallserial primary key,
	storage_name varchar (20) not null unique
);

-- drop table file_
create table file_
(
	file_guid serial primary key,
	storage_id smallint not null references storage_(storage_id) on update cascade,
	file_path varchar(255) not null
);


create table bmn_event
(
	file_guid int not null references file_(file_guid) on update cascade,
	event_number int not null check (event_number >= 0),
	primary key (file_guid, event_number),
	
	software_id smallint not null references software_(software_id) on update cascade,
	period_number smallint not null check (period_number >= 0),
	run_number int not null check (run_number >= 0),
	
	track_number int not null default (-1)
);

create table src_event
(
	file_guid int not null references file_(file_guid) on update cascade,
	event_number int not null check (event_number >= 0),
	primary key (file_guid, event_number),
	
	software_id smallint not null references software_(software_id) on update cascade,
	period_number smallint not null check (period_number >= 0),
	run_number int not null check (run_number >= 0),
	
	track_number int not null default (-1),
	input_charge real not null default (-1),
	output_charge real not null default (-1)
);