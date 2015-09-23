--База данных с одной таблицей - параметрами геометрии детекторов MPD
--drop database mpd_geo
create database mpd_geo;

--адаптация к MS SQL Server
--replace serial : int identity
--replace references : foreign key references
--replace timestamp(0) : smalldatetime
CREATE TABLE geo_parameter(
 parameter_id serial PRIMARY KEY,
 detector_name varchar(30) not null,
 parameter_name varchar(30) not null,
 parameter_type int not null default (0), -- 0 - double value, 1 - int value
 parameter_value double precision not null,
 description varchar(100) null,
 unique (detector_name, parameter_name)
);

--Global dimensions for TPC in mm
insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'tpc_inner_radius', 0, 270.0, 'tpc inner radius, mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'tpc_outer_radius', 0, 1405.0, 'tpc outer radius, mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'tpc_z', 0, 2000.0, 'tpc length, 0.5*4000.0 mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'tpc_chamber_z', 0, 1700.0, '0.5*3400.0 mm');


insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'membrane_thick', 0, 4.0, 'honeycomb, 0.5*8 mm');


insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'thick_al', 0, 0.05, 'for barrel walls, mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'thick_tedlar', 0, 0.05, 'for barrel walls, mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'kevlar', 0, 3.0, 'for barrel walls, mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'CO_2_gap_out', 0, 67.0, 'for barrel walls, mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'CO_2_gap_in', 0, 65.0, 'for barrel walls, mm');


insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'outer_flanch_width', 0, 268.0, 'flanches inside tpc_chamber_z, mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'inner_flanch_width', 0, 132.0, 'flanches inside tpc_chamber_z, mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'flanch_thickness', 0, 25.0, 'mm');


insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value)
values ('tpc', 'Nsect', 1, 12);


insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'fieldcage_out_rmax', 0, 1250.9, 'mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'fieldcage_out_rmin', 0, 1250.85, 'mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'fieldcage_in_rmax', 0, 371.0, 'mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'fieldcage_in_rmin', 0, 370.95, 'mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'fieldcage_r_out', 0, 1264.0, 'mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'fieldcage_r_in', 0, 377.2, 'mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'fieldcage_pin_r_out', 0, 30.0, 'pins for fieldcage, mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'fieldcage_pin_r_in', 0, 6.7, 'pins for fieldcage, mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'fieldcage_wall_thick', 0, 3.0, 'mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'fieldcage_phi_shift', 0, 0.122173048, '7.*TMath::DegToRad()');


insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'sv_X', 0, 320.8, 'sensitive volume trapezoid, 0.5*641.6 mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'sv_x_s', 0, 106.45, 'sensitive volume trapezoid, 0.5*212.9 mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'sv_Y', 0, 400.0, 'sensitive volume trapezoid, 0.5*800 mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'sv_Y_center', 0, 803.0, 'sensitive volume trapezoid, mm');


insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'rib_width_x', 0, 40.0, 'ribs, mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'rib_width_z', 0, 40.0, 'ribs, mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'rib_pos_z', 0, 60.0, 'distance from flanches, mm');


insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'zG10', 0, 2.8, 'FEE simulation, mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'zCu', 0, 0.822, 'FEE simulation, mm');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'zAl', 0, 1.0, 'FEE simulation, cooling, mm');


insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'pad_plane', 0, 3.0, 'pad plane simulation');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'back_plane_Al', 0, 4.0, 'pad plane simulation');

insert into geo_parameter(detector_name, parameter_name, parameter_type, parameter_value, description)
values ('tpc', 'back_plane_G10', 0, 3.0, 'pad plane simulation');
