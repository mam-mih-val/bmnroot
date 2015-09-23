--База данных MPD
--drop database mpd_db
create database mpd_db;

--адаптация к MS SQL Server
--replace serial : int identity
--replace references : foreign key references
--replace timestamp(0) : smalldatetime
create table detector(
 detector_id serial primary key,
 detector_name varchar(30) unique,
 description varchar(100) null
);

create table component_type(
 type_name varchar(30) primary key,
 description varchar(100) null
);

create table manufacturer(
 manufacturer_id serial primary key,
 maufacturer_parent_id int references manufacturer(manufacturer_id),
 
 manufacturer_name varchar(30) unique,
 isLaboratory bit not null,
 description varchar(100) null
);

create table media(
 media_name varchar(30) primary key,
 description varchar(100) null
);

create table geo_shape(
 shape_name varchar(30) primary key,
 description varchar(100) null
);

create table component(
 component_id serial primary key,
 detector_id int references detector(detector_id) not null,
 component_parent_id int references component(component_id),
 
 component_name varchar(30) not null,
 type_name varchar(30) references component_type(type_name),

 media_name varchar(30) references media(media_name),
 shape_name varchar(30) references geo_shape(shape_name),
 isVirtual bit not null,
 begin_part timestamp(0) not null,
 end_part timestamp(0) null,

 manufacturer_id int references manufacturer(manufacturer_id),
 production_date date null,
 description varchar(100) null
);

create table geo_point(
 component_id int references component(component_id),
 point_pos int not null,
 x float not null,
 y float not null,
 z float not null,

 primary key (component_id, point_pos)
);

--parameter_type: 0 - радиус, 1- длина
create table geo_parameter(
 component_id int references component(component_id),
 parameter_type int not null,
 parameter_value float not null,
 
 primary key (component_id, parameter_type)
);
