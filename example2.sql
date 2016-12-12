create table countries (
	pk_country serial,
	title      text not null,

	primary key (pk_country)
);

create table states (
	pk_state   serial,
	fk_country integer not null,
	title      text    not null,

	primary key (pk_state),

	foreign key (fk_country) references countries(pk_country)
		on update cascade on delete cascade
);

create table cities (
	pk_city  serial,
	fk_state integer not null,
	title    text    not null,

	primary key (pk_city),

	foreign key (fk_state) references states(pk_state)
		on update cascade on delete cascade
);

create table streets (
	pk_street serial,
	fk_city   integer not null,
	title     text    not null,

	primary key (pk_street),

	foreign key (fk_city) references cities(pk_city)
		on update cascade on delete cascade
);

create table addresses (
	pk_address    serial,
	fk_street     integer not null,
	house_no      text    not null,
	apartment_no  text, 

	primary key (pk_address),

	foreign key (fk_street) references streets(pk_street)
		on update cascade on delete cascade
);

create table genders (
	pk_gender serial,
	title     text not null,

	primary key (pk_gender)
);

create table ufms (
	pk_ufms   serial,
	title     text    not null,
	fk_address integer not null,

	primary key (pk_ufms),

	foreign key (fk_address) references addresses(pk_address)
		on update cascade on delete restrict
);

create table passports (
	pk_passport        serial,
	serial_no          integer not null,
	no                 integer not null,
	open_date          date    not null,
	fk_ufms_instance   integer not null,
	fk_official_address integer,

	primary key (pk_passport),

	foreign key (fk_ufms_instance) references ufms(pk_ufms)
		on update cascade on delete restrict,

	foreign key (fk_official_address) references addresses(pk_address)
		on update cascade on delete set null
);

create table persons (
	pk_person   serial,
	surname     text    not null,
	name        text    not null,
	middle_name text,
	birth_date  date    not null,
	fk_gender   integer not null,
	fk_passport integer,

	primary key (pk_person),

	foreign key (fk_gender) references genders(pk_gender)
		on update cascade on delete restrict,

	foreign key (fk_passport) references passports(pk_passport)
		on update cascade on delete set null
);

create table zags (
	pk_zags   serial,
	title     text    not null,
	fk_address integer not null,

	primary key (pk_zags),

	foreign key (fk_address) references addresses(pk_address)
		on update cascade on delete restrict
);

create table death_causes (
	pk_death_cause serial,
	title          text not null,

	primary key (pk_death_cause)
);

create table death_certificates (
	pk_death_certificate serial,
	certificate_no       text    not null,
	open_date            date    not null,
	fk_zags_instance     integer not null,
	fk_person            integer not null,
	death_date           date    not null,
	fk_death_cause       integer,

	primary key (pk_death_certificate),

	foreign key (fk_zags_instance) references zags(pk_zags)
		on update cascade on delete restrict,

	foreign key (fk_person) references persons(pk_person)
		on update cascade on delete restrict,

	foreign key (fk_death_cause) references death_causes(pk_death_cause)
		on update cascade on delete set null
);

create table graves (
	pk_grave serial,
	block_no integer not null,
	row_no   integer not null,
	place_no integer not null,

	primary key (pk_grave)
);

create table dead_persons (
	pk_dead_person       serial,
	fk_death_certificate integer not null,
	funeral_date         date,
	fk_grave             integer,

	primary key (pk_dead_person),

	foreign key (fk_death_certificate) references death_certificates(pk_death_certificate)
		on update cascade on delete restrict,

	foreign key (fk_grave) references graves(pk_grave)
		on update cascade on delete set null
);


create table professions (
	pk_profession serial,
	title         text    not null,
	salary        numeric not null,

	primary key (pk_profession)
);

create table workers (
	pk_worker      serial,
	fk_person      integer not null,
	phone          text,
	fk_profession  integer not null,
	fk_address     integer,

	primary key (pk_worker),

	foreign key (fk_person) references persons(pk_person)
		on update cascade on delete cascade,

	foreign key (fk_profession) references professions(pk_profession)
		on update cascade on delete cascade,

	foreign key (fk_address) references addresses(pk_address)
		on update cascade on delete set null
);

create table funerals (
	pk_funeral     serial,
	fk_worker      integer not null,
	fk_dead_person integer not null,

	primary key (pk_funeral),

	foreign key (fk_worker) references workers(pk_worker)
		on update cascade on delete cascade,

	foreign key (fk_dead_person) references dead_persons(pk_dead_person)
		on update cascade on delete cascade
);