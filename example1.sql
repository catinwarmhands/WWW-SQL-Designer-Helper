create table sides(
	pk_side integer,
	title varchar(50) not null,

	primary key (pk_side)
);

create table countries(
	pk_country integer,
	title varchar(50) not null,

	primary key (pk_country)
);

create table cities(
	pk_city integer,
	title varchar(50) not null,
	fk_country integer not null,

	primary key (pk_city),

	foreign key (fk_country) references countries(pk_country)
		on update cascade on delete restrict,
);

create table streets(
	pk_street integer,
	title varchar(50) not null,
	fk_city integer not null,

	primary key (pk_street),

	foreign key (fk_city) references cities(pk_city)
		on update cascade on delete restrict,
);

create table addresses(
	pk_address integer,
	fk_street integer not null,
	house_no varchar(50) not null,
	apartment_no varchar(50),

	primary key (pk_address),

	foreign key (fk_street) references streets(pk_street)
		on update cascade on delete restrict,
);

create table ufms(
	pk_ufms integer,
	no integer not null,
	fk_address integer not null,

	primary key (pk_ufms),

	foreign key (fk_address) references addresses(pk_address)
		on update cascade on delete restrict,
);

create table gangs(
	pk_gang integer,
	title varchar(50) not null,
	fk_side integer not null,
	fk_hq integer,

	primary key (pk_gang),

	foreign key (fk_side) references sides(pk_side)
		on update cascade on delete restrict,

	foreign key (fk_hq) references UNFOUND(pk_UNFOUND)
		on update cascade on delete restrict,
);

create table passports(
	pk_passport integer,
	no integer not null,
	fk_ufms integer not null,
	open_date date not null,

	primary key (pk_passport),

	foreign key (fk_ufms) references ufms(pk_ufms)
		on update cascade on delete restrict,
);

create table mutants(
	pk_mutant integer,
	name varchar(50) not null,
	surname varchar(50),
	middle_name varchar(50),
	birth_date date not null,
	nickname varchar(50),
	fk_gang integer,
	fk_passport integer,

	primary key (pk_mutant),

	foreign key (fk_gang) references gangs(pk_gang)
		on update cascade on delete restrict,

	foreign key (fk_passport) references passports(pk_passport)
		on update cascade on delete restrict,
);

create table activity_types(
	pk_activity_type integer,
	title varchar(50) not null,

	primary key (pk_activity_type)
);

create table activities(
	pk_activity integer,
	fk_activity_type integer not null,
	activity_date date not null,

	primary key (pk_activity),

	foreign key (fk_activity_type) references activity_types(pk_activity_type)
		on update cascade on delete restrict,
);

create table activity_members(
	pk_activity_member integer,
	fk_activity integer not null,
	fk_mutant integer not null,

	primary key (pk_activity_member),

	foreign key (fk_activity) references activities(pk_activity)
		on update cascade on delete restrict,

	foreign key (fk_mutant) references mutants(pk_mutant)
		on update cascade on delete restrict,
);

create table power_causes(
	pk_power_cause integer,
	title varchar(50) not null,

	primary key (pk_power_cause)
);

create table powers(
	pk_power integer,
	title varchar(50) not null,
	fk_power_cause integer not null,

	primary key (pk_power),

	foreign key (fk_power_cause) references power_causes(pk_power_cause)
		on update cascade on delete restrict,
);

create table mutant_powers(
	pk_mutant_power integer,
	fk_mutant integer not null,
	fk_power integer not null,

	primary key (pk_mutant_power),

	foreign key (fk_mutant) references mutants(pk_mutant)
		on update cascade on delete restrict,

	foreign key (fk_power) references powers(pk_power)
		on update cascade on delete restrict,
);