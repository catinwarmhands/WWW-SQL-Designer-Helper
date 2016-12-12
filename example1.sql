create table sides(
	pk_side integer,
	title varchar(50) not null
);

create table countries(
	pk_country integer,
	title varchar(50) not null
);

create table cities(
	pk_city integer,
	title varchar(50) not null,
	fk_country integer not null
);

create table streets(
	pk_street integer,
	title varchar(50) not null,
	fk_city integer not null
);

create table addresses(
	pk_address integer,
	fk_street integer not null,
	house_no varchar(50) not null,
	apartment_no varchar(50)
);

create table ufms(
	pk_ufms integer,
	no integer not null,
	fk_address integer not null
);

create table gangs(
	pk_gang integer,
	title varchar(50) not null,
	fk_side integer not null,
	fk_hq integer
);

create table passports(
	pk_passport integer,
	no integer not null,
	fk_ufms integer not null,
	open_date date not null
);

create table mutants(
	pk_mutant integer,
	name varchar(50) not null,
	surname varchar(50),
	middle_name varchar(50),
	birth_date date not null,
	nickname varchar(50),
	fk_gang integer,
	fk_passport integer
);

create table activity_types(
	pk_activity_type integer,
	title varchar(50) not null
);

create table activities(
	pk_activity integer,
	fk_activity_type integer not null,
	activity_date date not null
);

create table activity_members(
	pk_activity_member integer,
	fk_activity integer not null,
	fk_mutant integer not null
);

create table power_causes(
	pk_power_cause integer,
	title varchar(50) not null
);

create table powers(
	pk_power integer,
	title varchar(50) not null,
	fk_power_cause integer not null
);

create table mutant_powers(
	pk_mutant_power integer,
	fk_mutant integer not null,
	fk_power integer not null
);
