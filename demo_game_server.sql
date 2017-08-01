create database if not exists game_server charset=utf8;

use game_server;

create table if not exists users (
	id int not null AUTO_INCREMENT primary key,
	name varchar(18) not null unique key,
	password varchar(18) not null
)ENGINE = InnoDB CHARSET=utf8;
insert into users(`name`,`password`) values('mike','mike123');
insert into users(`name`,`password`) values('tom','tom123');
insert into users(`name`,`password`) values('jerry','jerry123');
insert into users(`name`,`password`) values('123456','123456123');

