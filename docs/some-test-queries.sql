drop database if exists termgram;
create database termgram;
use termgram;

-- run tables-safe.sql

insert into Chat(`id`, `name`, `description`) values
		    (1, 'chat a', 'description of chat a'),
            (2, 'chat b', 'description of chat b'),
            (3, 'chat c', 'description of chat c'),
            (4, 'chat d', 'description of chat d'),
            (5, 'chat e', 'description of chat e'),
            (6, 'chat f', 'description of chat f'),
            (7, 'chat g', 'description of chat g'),
            (8, 'chat h', 'description of chat h'),
            (9, 'chat i', 'description of chat i');

insert into Address(`id`, `host`, `port`) values
			(1, '192.168.0.1', '8080'),
            (2, '192.168.0.2', '8080'),
            (3, '192.168.0.3', '8080'),
            (4, '192.168.0.4', '8080'),
            (5, '192.168.0.5', '8080'),
            (6, '192.168.0.6', '8080'),
            (7, '192.168.0.7', '8080'),
            (8, '192.168.0.8', '8080'),
            (9, '192.168.0.9', '8080');

insert into `User`(`id`, `name`, `user_address`) values
			(1, 'user a', 1),
            (2, 'user b', 2),
            (3, 'user c', 3),
            (4, 'user d', 4),
            (5, 'user e', 5),
            (6, 'user f', 6),
            (7, 'user g', 7),
            (8, 'user h', 8),
            (9, 'user i', 9);

insert into `Chat_has_User`(Chat_id, User_id) values
			(1, 1),
            (1, 2);

insert into `Chat_has_User`(Chat_id, User_id) values
			(2, 3),
            (2, 5);
            
insert into `Chat_has_User`(Chat_id, User_id) values
			(3, 1),
            (3, 2),
            (3, 3),
            (3, 4);

select U.id, U.`name`, A.`host`, A.`port` from `User` as U
			  join Address as A on U.user_address = A.id
			  join `Chat_has_User` as CU on CU.User_id = U.id
              where CU.Chat_id = 3;
              
              
              
              
              
              